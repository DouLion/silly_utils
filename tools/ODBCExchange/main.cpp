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

    std::string conn = R"({"type": "mysql",
"ip": "192.168.0.73",
"port": 3306,
"driver": "MYSQL ODBC 9.1 UNICODE Driver",
"schema": "TZX_FloodDisaster_HN",
"user": "root",
"password": "3edc9ijn~"
})";
    std::string sql = R"(select * from MessageInfo_R;)";

    X::Table table;
    table.otl.load(conn);


    otl_connect db;
    otl_stream stream;
    std::string m_err;
    try
    {
        db.auto_commit_off();
        db.set_timeout(5);
        db.rlogon(table.otl.odbc().c_str(), false);

        stream.open(1, sql.c_str(), db);

        table.Read(&stream);

        stream.close();
    }
    catch (otl_exception& e)
    {

        db.rollback();
        m_err = "OTL_ERR \nCONN:";
        m_err.append(table.otl.odbc());
        m_err.append("\nCODE:").append(std::to_string(e.code));
        m_err.append("\nMSG:").append(std::string((char*)e.msg));
        m_err.append("\nSTATE:").append(std::string((char*)e.sqlstate));
        m_err.append("\nSTMT:").append(std::string((char*)e.stm_text));
    }
    catch (std::exception& p)
    {
        db.rollback();
        m_err = "OTL_UNKNOWN " + std::string(p.what());
    }
    stream.close();
    db.logoff();
    if(!m_err.empty())
    {
        SLOG_ERROR(m_err);
        return -1;
    }

    table.Print();

    return 0;
}