//
// Created by dell on 2023/12/21.
//
#pragma once
#include "silly_otl.h"
#include "su_marco.h"
#include "json/silly_jsonpp.h"


const static std::string SILLY_OTL_MYSQL_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;Database=%s;User=%s;Password=%s;Option=3;charset=UTF8;";
const static std::string SILLY_OTL_MSSQL_ODBC_FORMAT = "Driver={%s};Server=%s;Port:%d;Database=%s;UID=%s;PWD=%s;";
const static std::string SILLY_OTL_ORACLE_ODBC_FORMAT = "Driver={%s};DBQ=%s:%d/%s;Uid=%s;Pwd=%s;";
const static std::string SILLY_OTL_DM8_ODBC_FORMAT = "Driver={%s};Server=%s;TCP_PORT=%d;UID=%s;PWD=%s;";
const static std::string SILLY_OTL_POSTGRE_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;Database=%s;Uid=%s;Pwd=%s;";
const static std::string SILLY_OTL_DSN_FORMAT = "UID=%s;PWD=%s;DSN=%s;";

bool otl_conn_opt::load(const std::string& s_opt)
{
    clean();
    bool status = false;
    if (s_opt.empty())
    {
        return status;
    }

    Json::Value jv_root;
    if (!(jv_root = silly_jsonpp::loads(s_opt)).isNull())
    {
        silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_DSN, m_dsn);
        if (m_dsn.empty())  // 非DSN方式
        {
            // 检查类型
            std::string type_str;
            if (!silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_TYPE, type_str))
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
            if (enum_database_type::dbKingB8 == m_type)
            {
                m_err = "人大金仓请使用DSN方式(Please set DSN when using Kingbase).";
                // SLOG_ERROR("达梦和人大金仓请使用DSN方式(Please set DSN when using Dameng or Kingbase).");
                return status;
            }

            if (!silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_IP, m_ip))
            {
                m_err = "未指定IP";
                return status;
            }
            if (!silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_DRIVER, m_driver))
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
                    case enum_database_type::dbDM8:
                        m_port = 5236;
                        break;
                    default:
                        break;
                }
            }

            if (!silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_SCHEMA, m_schema))
            {
                m_err = "未指定数据库";
                return status;
            }
        }
        if (!silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_USER, m_user))
        {
            m_err = "未指定用户名";
            return status;
        }
        if (!silly_jsonpp::check_member_string(jv_root, SILLY_OTL_OPT_S_PASSWORD, m_password))
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
            sprintf(buff, SILLY_OTL_DSN_FORMAT.c_str(), m_user.c_str(), m_password.c_str(), m_dsn.c_str());
            m_conn = buff;
        }
        else if (!m_driver.empty())
        {
            switch (m_type)  // 使用ODBC连接串
            {
                case enum_database_type::dbMYSQL:
                    sprintf(buff, SILLY_OTL_MYSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case enum_database_type::dbSQLSERVER:
                    sprintf(buff, SILLY_OTL_MSSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case enum_database_type::dbORACLE:
                    sprintf(buff, SILLY_OTL_ORACLE_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case enum_database_type::dbPG:
                    sprintf(buff, SILLY_OTL_POSTGRE_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case enum_database_type::dbDM8:
                    sprintf(buff, SILLY_OTL_DM8_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_user.c_str(), m_password.c_str());
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
        db.rollback();
        m_err = silly_format::format("OTL_ERR \nCONN:{} \nCODE:{} \nMSG:{} \nSTATE:{}\n", m_conn, e.code, (char*)e.msg, (char*)e.sqlstate);
    }
    catch (std::exception& p)
    {
        db.rollback();
        m_err = silly_format::format("OTL_UNKNOWN{}\n", p.what());
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
        "\nOTL 连接串帮助信息:\n >>> 账号和密码中不要出现 [ ] { } ( ) , ; ? * = ! @ | 这些特殊字符 <<<\nSQL Server:\n\tDRIVER={驱动名称};SERVER=IP;PORT=端口;UID=账号;PWD=密码;DATABASE=数据库;\nMySQL:\n\tDriver={MySQL ODBC 8.0 ANSI "
        "Driver};Server=IP;Port=端口;Database=数据库;User=账号;Password=密码;Option=3;charset=UTF8;\nOracle:\n\tDriver={ODBC驱动名称};DBQ=IP:端口/表空间名称;UID=用户;PWD=密码;Oracle需要另外设置环境变量NLS_LANG=SIMPLIFIED "
        "CHINESE_CHINA.UTF8,以支持中文编码utf8传递;\n达梦(DM8):\n\tDriver={驱动名称};Server=IP;TCP_PORT:端口;UID=账号;PWD=密码; \n\t即使数据库编码为UTF8, 数据在插入时也需要时GBK编码, 否则会乱码;"
        "\n不能正常使用ODBC时,考虑使用DSN方式:\n\tUID=账号;PWD=密码;DSN=DNS名称;\n")
    std::string content = "\n\n当前机器支持的ODBC驱动:\n";
    for( auto d: drivers())
    {
        content += d + "\n";
    }

    SLOG_INFO(content)
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
#ifdef IS_WIN32
#include <odbcinst.h>
#include <cstring>
#pragma  comment(lib, "odbccp32.lib")
#pragma  comment(lib, "legacy_stdio_definitions.lib")
#endif
std::vector<std::string> otl_conn_opt::drivers()
{
    std::vector<std::string> retVec;
#ifdef IS_WIN32

    WCHAR szBuf[10240] = { 0 };
    WORD cbBufMax = 10239;
    WORD cbBufOut;
    WCHAR* pszBuf = szBuf;
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    if (SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
    {
        do
        {
            pszBuf = wcschr(pszBuf, '\0') + 1;
            retVec.push_back(converterX.to_bytes(pszBuf));
        } while (pszBuf[1] != '\0');
    }
#else
    FILE* fp;
    char buffer[4096];
    fp = popen("odbcinst -q -d", "r");
    while (nullptr != fgets(buffer, 4096, fp))
    {
        // printf("%s", buffer);
        std::string tmp_odbc_driver(buffer);
        tmp_odbc_driver = tmp_odbc_driver.substr(1, tmp_odbc_driver.size() - 3); // 每一行的结果 [MySQL ODBC 8.0 Unicode Driver]\r    最后有个换行符,所以是 -3
        retVec.push_back(tmp_odbc_driver);
        memset(buffer, 0, 4096);
    }

    pclose(fp);
#endif
    return retVec;
}
