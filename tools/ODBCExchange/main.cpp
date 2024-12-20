/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-14
 * @file: main.c
 * @description: Odbc 数据交换
 *             1. 通过中间文件实现数据交换
 *             2. 支持任意表、任意字段, 任意数据库类型(mysql, sqlserver, oracle, dm8), 目前主要是这四个
 * @version: v1.0.1 2024-12-14 dou li yang
 */
#include <log/silly_log.h>
#include <Exchange.h>

int main(int argc, char** argv)
{
    if (!silly_log::instance().init(argc, argv))
    {
        std::cerr << "日志模块初始化失败" << std::endl;
        exit(1);
    }

#ifndef NDEBUG
    std::string root = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("docs").append("ODBCExchange").string();
    // 切换当前工作目录
    std::filesystem::current_path(root);
#endif
    std::string tableName = R"("TZX_FloodDisaster_XJ_FWQ"."Dzwl_MessageInfo_R")";
    if(argc == 2)
    {
        tableName = argv[1];
    }

    std::string conn = R"({"type": "dm8",
"ip": "192.168.0.156",
"port": 5237,
"driver": "DM8 ODBC DRIVER",
"schema": "TZX_FloodDisaster_XJ_FWQ",
"user": "SYSDBA",
"password": "3edc9ijn~"
})";
    std::string sql = R"(select * from )" + tableName + R"( limit 20)";
    SLOG_INFO("sql: {}", sql)

    X::Table table;
    if (!table.Connect(conn))
    {
        return 2;
    }

    table.Pull(sql);

    table.Print();

    return 0;
}