//
// Created by dell on 2023/12/21.
//
#pragma once
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

    Json::Value jv_root;
    if ((jv_root = silly_jsonpp::loads(s_opt)).isNull())
    {
        silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_DSN, m_dsn);
        if (m_dsn.empty())  // 非DSN方式
        {
            // 检查类型
            std::string type_str;
            if (silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_TYPE, type_str))
            {
                m_err = "指定链接类型";
                return status;
            }
            m_type = str_to_db_type(type_str);
            if (enum_database_type::dbINVALID == m_type)
            {
                m_err = silly_format::format("不支持的数据库类型 (Unsupported database type): {}.", type_str);
                return status;
            }
            if (enum_database_type::dbDM8 == m_type || enum_database_type::dbKingB8 == m_type)
            {
                m_err = "达梦和人大金仓请使用DSN方式(Please set DSN when using Dameng or Kingbase).";
                // SLOG_ERROR("达梦和人大金仓请使用DSN方式(Please set DSN when using Dameng or Kingbase).");
                return status;
            }

            if (silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_IP, m_ip))
            {
                m_err = "未指定IP";
                return status;
            }
            if (silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_DRIVER, m_driver))
            {
                m_err = "未指定驱动";
                return status;
            }

            // 端口
            if (jv_root.isMember(SILLY_OTL_OPT_S_PORT))
            {
                if (jv_root[SILLY_OTL_OPT_S_PORT].isInt())
                {
                    m_port = jv_root[SILLY_OTL_OPT_S_PORT].asInt();
                }
                else if (jv_root[SILLY_OTL_OPT_S_PORT].isString())
                {
                    m_port = std::stoi(jv_root[SILLY_OTL_OPT_S_PORT].asString());
                }
            }
            else
            {
                switch (m_type)
                {
                    case enum_database_type::dbSQLSERVER:
                        m_port = 1433;
                        break;
                    case enum_database_type::dbMYSQL:
                        m_port = 3306;
                        break;
                    case enum_database_type::dbORACLE:
                        m_port = 1521;
                        break;
                    case enum_database_type::dbPG:
                        m_port = 5432;
                        break;
                    default:
                        break;
                }
            }

            if (silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_SCHEMA, m_schema))
            {
                m_err = "未指定数据库";
                return status;
            }
        }
        if (silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_USER, m_user))
        {
            m_err = "未指定用户名";
            return status;
        }
        if (silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_PASSWORD, m_password))
        {
            m_err = "未指定密码";
            return status;
        }
        m_conn = odbc(true);
    }
    else if (!s_opt.empty())  // 纯字符串
    {
        m_conn = s_opt;
    }

    return check();
}

std::string otl_conn_opt::odbc(const bool& rebuild)
{
    return dump_odbc(rebuild);
}

std::string otl_conn_opt::dump_odbc(const bool& rebuild)
{
    if (m_conn.empty() || rebuild)
    {
        char buff[SILLY_OTL_ODBC_MAX_LEN] = {0};
        if (!m_dsn.empty())  // 如果ODBC连接串不好使,设置DSN,并且优先使用DSN链接方式
        {
            sprintf(buff, SILLY_OTL_DSN_FORMAT, m_user.c_str(), m_password.c_str(), m_dsn.c_str());
            m_conn = buff;
        }
        else if (!m_driver.empty())
        {
            switch (m_type)  // 使用ODBC连接串
            {
                case enum_database_type::dbMYSQL:
                    sprintf(buff, SILLY_OTL_MYSQL_ODBC_FORMAT, m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case enum_database_type::dbSQLSERVER:
                    sprintf(buff, SILLY_OTL_MSSQL_ODBC_FORMAT, m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case enum_database_type::dbORACLE:
                    sprintf(buff, SILLY_OTL_ORACLE_ODBC_FORMAT, m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case enum_database_type::dbPG:
                    sprintf(buff, SILLY_OTL_POSTGRE_ODBC_FORMAT, m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                default:
                    break;
            }
            m_conn = buff;
        }
    }
    return m_conn;
}

bool otl_conn_opt::check()
{
    bool status = false;
    otl_connect db;
    try
    {
        db.rlogon(m_conn.c_str());
        status = true;
    }
    catch (otl_exception& e)
    {
        m_err = silly_format::format("打开链接失败, \nsql:%s \nmessage:%s \n state:%s", (char*)e.stm_text, (char*)e.msg, (char*)e.sqlstate);
        // SLOG_ERROR("查询失败, \nsql:%s \nmessage:%s \n state:%s", e.stm_text, e.msg, e.sqlstate);
    }
    catch (const std::exception& p)
    {
        m_err = p.what();
        SLOG_ERROR("%s", p.what());
    }
    db.logoff();
    return status;
}

void otl_conn_opt::clean()
{
    m_ip.clear();
    m_port = 0;
    m_type = enum_database_type::dbINVALID;
    m_driver.clear();
    m_schema.clear();
    m_user.clear();
    m_password.clear();
    m_dsn.clear();
    m_conn.clear();
    m_err.clear();
}

void otl_conn_opt::help()
{
    SLOG_INFO(
        "OTL 连接串帮助信息:\nSQL Server:\n\tDRIVER={驱动名称};SERVER=IP;PORT=端口;UID=账号;PWD=密码;DATABASE=数据库;\nMySQL:\n\tDriver={MySQL ODBC 8.0 ANSI "
        "Driver};Server=IP;Port=端口;Database=数据库;User=账号;Password=密码;Option=3;charset=UTF8;\nOracle:\n\tDriver={ODBC驱动名称};DBQ=IP:端口/表空间名称;UID=用户;PWD=密码;Oracle需要另外设置环境变量NLS_LANG=SIMPLIFIED CHINESE_CHINA.UTF8,以支持中文编码utf8传递;\n达梦(DM8):\n\t达梦数据库目前只能使用DSN的方式  "
        "UID=用户;PWD=密码;DSN=DSN名称;\n")
}

static char* sqlserver_code_sql = "SELECT COLLATIONPROPERTY('Chinese_PRC_Stroke_CI_AI_KS_WS', 'CodePage');";
static std::map<std::string, std::string> sqlserver_code_map = {{"936", "GBK"}, {"950", "BIG5"}, {"437", "Eng"}, {"932", "JP"}, {"949", "KOREA"}, {"866", "RUSSIA"}, {"65001", "UFT-8"}, {"", "INVALID"}};

std::string otl_conn_opt::encode()
{
    std::string result;
    std::string code;
    otl_connect db;
    try
    {
        m_conn = dump_odbc(true);
        db.rlogon(m_conn.c_str());
        char buff[512] = {0};
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
        SLOG_ERROR("查询失败, \nsql:%s \nmessage:%s \n state:%s", e.stm_text, e.msg, e.sqlstate);
    }
    catch (const std::exception& p)
    {
        SLOG_ERROR("%s", p.what());
    }
    db.logoff();
    result = sqlserver_code_map[code];
    return result;
}

enum_database_type otl_conn_opt::type() const
{
    return m_type;
}

std::string otl_conn_opt::driver() const
{
    return m_driver;
}

std::string otl_conn_opt::ip() const
{
    return m_ip;
}

int otl_conn_opt::port() const
{
    return m_port;
}

std::string otl_conn_opt::schema() const
{
    return m_schema;
}

std::string otl_conn_opt::user() const
{
    return m_user;
}

std::string otl_conn_opt::pwd() const
{
    return m_password;
}

std::string otl_conn_opt::err() const
{
    return m_err;
}

void otl_conn_opt::type(enum_database_type tp)
{
    m_type = tp;
}

void otl_conn_opt::driver(std::string d)
{
    m_driver = d;
}

void otl_conn_opt::ip(std::string i)
{
    m_ip = i;
}

void otl_conn_opt::port(int p)
{
    m_port = p;
}

void otl_conn_opt::schema(std::string s)
{
    m_schema = s;
}

void otl_conn_opt::user(std::string u)
{
    m_user = u;
}

void otl_conn_opt::pwd(std::string p)
{
    m_password = p;
}

void otl_conn_opt::timeout(int to)
{
    m_timeout = to;
}
