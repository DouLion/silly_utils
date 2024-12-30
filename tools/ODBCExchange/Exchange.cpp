/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-14
 * @file: Exchange.cpp
 * @description: Exchange实现
 * @version: v1.0.1 2024-12-14 dou li yang
 */
#include "Exchange.h"
#include <database/otl/otl_tools.h>

std::string CFG_EXAMPLE = R"(
{
  "db": {
    "type": "dm8",
    "ip": "127.0.0.1",
    "port": 5237,
    "driver": "DM8 ODBC DRIVER",
    "schema": "表空间名或者库名",
    "user": "账号",
    "password": "密码"
  },
  "sql": "select * from \"TZX_FloodDisaster_XJ_FWQ\".\"WarnRecord_R\"";
}
)";

bool X::PullOpt::load(const std::string& file)
{
    Json::Value root = silly_jsonpp::loadf(file);
    if (root.isNull())
    {
        SLOG_ERROR("加载配置文件错误: " + std::filesystem::absolute(file).string());
        return false;
    }
    Json::Value jvdb;
    if (!silly_jsonpp::check_member_object(root, "db", jvdb))
    {
        SLOG_ERROR("配置示例: " + CFG_EXAMPLE);
        return false;
    }
    if (!silly_jsonpp::check_member_string(root, "sql", sql))
    {
        SLOG_ERROR("配置示例: " + CFG_EXAMPLE);
        return false;
    }

    otl.from_json(jvdb);

    SLOG_INFO("ODBC: " + otl.odbc(true));
    if (!otl.check())
    {
        SLOG_ERROR("ODBC配置错误: " + otl.err());
    }

    SLOG_INFO("SQL: " + sql)
    return true;
}
