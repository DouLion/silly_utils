//
// Created by dell on 2023/12/21.
//

#include "silly_otl.h"
#include "su_marco.h"
#include "json/silly_jsonpp.h"

bool otl_conn_opt::load(const std::string& s_opt)
{
    clean();
    bool status = false;
    if (s_opt.empty())
    {
        return status;
    }

    Json::Value jv_root = silly_jsonpp::loads(s_opt);
    if (jv_root.isObject())
    {
        if (jv_root.isMember(SILLY_OTL_OPT_S_IP))
        {
            ip = jv_root[SILLY_OTL_OPT_S_IP].asString();
        }
        if (jv_root.isMember(SILLY_OTL_OPT_S_TYPE))
        {
            std::string s_type = jv_root[SILLY_OTL_OPT_S_TYPE].asString();
            type = str_to_db_type(s_type);
            if (enum_database_type::dbINVALID == type)
            {
                SU_ERROR_PRINT("不支持的数据库类型 (Invalid database type).")
                return status;
            }
            if (enum_database_type::dbDM8 == type || enum_database_type::dbKingB8 == type)
            {
                SU_ERROR_PRINT("达梦和人大金仓请使用DSN方式(Please set DSN when using Dameng or Kingbase).")
                return status;
            }
        }
        if (jv_root.isMember(SILLY_OTL_OPT_S_PORT))
        {
            if (jv_root[SILLY_OTL_OPT_S_PORT].isInt())
            {
                port = jv_root[SILLY_OTL_OPT_S_PORT].asInt();
            }
            else if (jv_root[SILLY_OTL_OPT_S_PORT].isString())
            {
                port = std::stoi(jv_root[SILLY_OTL_OPT_S_PORT].asString());
            }
        }
        else
        {
            switch (type)
            {
                case enum_database_type::dbSQLSERVER:
                    port = 1433;
                    break;
                case enum_database_type::dbMYSQL:
                    port = 3306;
                    break;
                case enum_database_type::dbORACLE:
                    port = 1521;
                    break;
                case enum_database_type::dbPG:
                    port = 5432;
                    break;
                default:
                    break;
            }
        }
        if (jv_root.isMember(SILLY_OTL_OPT_S_DRIVER) && jv_root[SILLY_OTL_OPT_S_DRIVER].isString())
        {
            driver = jv_root[SILLY_OTL_OPT_S_DRIVER].asString();
        }
        if (jv_root.isMember(SILLY_OTL_OPT_S_SCHEMA) && jv_root[SILLY_OTL_OPT_S_SCHEMA].isString())
        {
            schema = jv_root[SILLY_OTL_OPT_S_SCHEMA].asString();
        }
        if (jv_root.isMember(SILLY_OTL_OPT_S_USER) && jv_root[SILLY_OTL_OPT_S_USER].isString())
        {
            user = jv_root[SILLY_OTL_OPT_S_USER].asString();
        }
        if (jv_root.isMember(SILLY_OTL_OPT_S_PASSWORD) && jv_root[SILLY_OTL_OPT_S_PASSWORD].isString())
        {
            password = jv_root[SILLY_OTL_OPT_S_PASSWORD].asString();
        }
        if (jv_root.isMember(SILLY_OTL_OPT_S_DSN) && jv_root[SILLY_OTL_OPT_S_DSN].isString())
        {
            dsn = jv_root[SILLY_OTL_OPT_S_DSN].asString();
        }
        status = true;
    }
    else if (jv_root.isNull())  // 纯字符串
    {
        conn = s_opt;
        status = true;
    }
    else  // 其他json格式, 不支持
    {
    }

    return status;
}

std::string otl_conn_opt::dump_odbc(const bool& rebuild)
{
    if (conn.empty() || rebuild)
    {
        char buff[SILLY_OTL_ODBC_MAX_LEN] = {0};
        if (!dsn.empty())  // 如果ODBC连接串不好使,设置DSN,并且优先使用DSN链接方式
        {
            sprintf(buff, SILLY_OTL_DSN_FORMAT, user.c_str(), password.c_str(), dsn.c_str());
            conn = buff;
        }
        else if (!driver.empty())
        {
            switch (type)  // 使用ODBC连接串
            {
                case enum_database_type::dbMYSQL:
                    sprintf(buff, SILLY_OTL_MYSQL_ODBC_FORMAT, driver.c_str(), ip.c_str(), port, schema.c_str(), user.c_str(), password.c_str());
                    break;
                case enum_database_type::dbSQLSERVER:
                    sprintf(buff, SILLY_OTL_MSSQL_ODBC_FORMAT, driver.c_str(), ip.c_str(), port, schema.c_str(), user.c_str(), password.c_str());
                    break;
                case enum_database_type::dbORACLE:
                    sprintf(buff, SILLY_OTL_ORACLE_ODBC_FORMAT, driver.c_str(), ip.c_str(), port, schema.c_str(), user.c_str(), password.c_str());
                    break;
                case enum_database_type::dbPG:
                    sprintf(buff, SILLY_OTL_POSTGRE_ODBC_FORMAT, driver.c_str(), ip.c_str(), port, schema.c_str(), user.c_str(), password.c_str());
                    break;
                default:
                    break;
            }
            conn = buff;
        }
    }
    return conn;
}

bool otl_conn_opt::check()
{
    bool status = false;
    otl_connect db;
    try
    {
        conn = dump_odbc(true);
        db.rlogon(conn.c_str());
        status = true;
    }
    catch (otl_exception& e)
    {
        SU_ERROR_PRINT("查询失败, \nsql:%s \nmessage:%s \n state:%s", e.stm_text, e.msg, e.sqlstate);
    }
    catch (const std::exception& p)
    {
        SU_ERROR_PRINT("%s", p.what());
    }
    db.logoff();
    return status;
}

void otl_conn_opt::clean()
{
    ip.clear();
    port = 0;
    type = enum_database_type::dbINVALID;
    driver.clear();
    schema.clear();
    user.clear();
    password.clear();
    dsn.clear();
    conn.clear();
}

void otl_conn_opt::help()
{
    SU_INFO_PRINT(
        "OTL 连接串帮助信息:\nSQL Server:\n\tDRIVER={驱动名称};SERVER=IP;PORT=端口;UID=账号;PWD=密码;DATABASE=数据库;\nMySQL:\n\tDriver={MySQL ODBC 8.0 ANSI "
        "Driver};Server=IP;Port=端口;Database=数据库;User=账号;Password=密码;Option=3;charset=UTF8;\nOracle:\n\tDriver={ODBC驱动名称};DBQ=IP:端口/表空间名称;UID=用户;PWD=密码;\n达梦(DM8):\n\t达梦数据库目前只能使用DSN的方式  "
        "UID=用户;PWD=密码;DSN=DSN名称;\n")
}

static char* sqlserver_code_sql = "SELECT COLLATIONPROPERTY('Chinese_PRC_Stroke_CI_AI_KS_WS', 'CodePage');";
static std::map<std::string, std::string> sqlserver_code_map = { {"936", "GBK"}, {"950", "BIG5"}, {"437", "Eng"}, {"932", "JP"}, {"949", "KOREA"}, {"866", "RUSSIA"}, {"65001", "UFT-8"}, {"", "INVALID"}};

std::string otl_conn_opt::encode()
{
    std::string result;
    std::string code;
    otl_connect db;
    try
    {
        conn = dump_odbc(true);
        db.rlogon(conn.c_str());
        char buff[512] = { 0 };
        sprintf(buff, "%s", sqlserver_code_sql);
        otl_stream query_stream;
        query_stream.open(1, buff, db);
        for (auto& qs : query_stream)
        {
            otl_read_row(qs, code);
        }
        query_stream.close();
        db.logoff();
    }
    catch (otl_exception& e)
    {
        SU_ERROR_PRINT("查询失败, \nsql:%s \nmessage:%s \n state:%s", e.stm_text, e.msg, e.sqlstate);
    }
    catch (const std::exception& p)
    {
        SU_ERROR_PRINT("%s", p.what());
    }
    db.logoff();
    result = sqlserver_code_map[code];
    return result;
}
