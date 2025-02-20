//
// Created by dell on 2023/12/21.
//
#pragma once
#include "silly_otl.h"
using namespace silly::db;
const static std::string SILLY_OTL_MYSQL_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;Database=%s;User=%s;Password=%s;Option=3;charset=UTF8;";
const static std::string SILLY_OTL_MARIA_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;Database=%s;User=%s;Password=%s;Option=3;charset=UTF8;";
const static std::string SILLY_OTL_MSSQL_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;UID=%s;PWD=%s;Database=%s;";
const static std::string SILLY_OTL_ORACLE_ODBC_FORMAT = "Driver={%s};DBQ=%s:%d/%s;Uid=%s;Pwd=%s;";
const static std::string SILLY_OTL_DM8_ODBC_FORMAT = "Driver={%s};Server=%s;TCP_PORT=%d;UID=%s;PWD=%s;";
const static std::string SILLY_OTL_POSTGRE_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;Database=%s;Uid=%s;Pwd=%s;";
const static std::string SILLY_OTL_DSN_FORMAT = "UID=%s;PWD=%s;DSN=%s;";

#define TYPE_MSSQL_STR "sqlserver"
#define TYPE_MYSQL_STR "mysql"
#define TYPE_MARIA_STR "maria"
#define TYPE_ORACLE_STR "oracle"
#define TYPE_DM8_STR "dm8"
#define TYPE_POSTGRESQL_STR "postgresql"
#define TYPE_KING8_STR "kb8"

#define OPT_STR_IP "ip"
#define OPT_STR_PORT "port"
#define OPT_STR_TYPE "type"
#define OPT_STR_DRIVER "driver"
#define OPT_STR_SCHEMA "schema"
#define OPT_STR_USER "user"
#define OPT_STR_PASSWORD "password"
#define OPT_STR_DSN "dsn"
#define OPT_STR_VERBOSE "verbose"

// 去除字符串左边的空格
std::string ltrim(const std::string& str)
{
    auto it = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
    return std::string(it, str.end());
}

// 去除字符串右边的空格
std::string rtrim(const std::string& str)
{
    auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); }).base();
    return std::string(str.begin(), it);
}

// 去除字符串左右两边的空格
std::string _trim(const std::string& str)
{
    return ltrim(rtrim(str));
}

std::string _lower(const std::string& str)
{
    std::string ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) { return std::tolower(c); });
    return ret;
}

/// <summary>
/// 根据驱动名称,猜测数据库类型
/// </summary>
/// <param name="driver"></param>
/// <returns></returns>
static otl::eType assume_type(const std::string& driver)
{
    std::string lower_driver = driver;
    std::transform(lower_driver.begin(), lower_driver.end(), lower_driver.begin(), ::tolower);
    if (lower_driver.find("sql server") != std::string::npos)
    {
        return otl::eType::dbSQLSERVER;
    }
    else if (lower_driver.find("mysql") != std::string::npos)
    {
        return otl::eType::dbMYSQL;
    }
    else if (lower_driver.find("oracle") != std::string::npos)
    {
        return otl::eType::dbORACLE;
    }
    else if (lower_driver.find("postgresql") != std::string::npos)
    {
        return otl::eType::dbPG;
    }
    else if (lower_driver.find("dm8") != std::string::npos)
    {
        return otl::eType::dbDM8;
    }
    else if (lower_driver.find("maria") != std::string::npos)
    {
        return otl::eType::dbKingB8;
    }

    return otl::eType::dbINVALID;
}

static std::map<std::string, std::string> parse_odbc(const std::string& odbc)
{
    std::map<std::string, std::string> result;
    std::istringstream iss(odbc);
    std::string token;

    while (std::getline(iss, token, ';'))
    {
        size_t pos = token.find('=');
        if (pos != std::string::npos)
        {
            std::string key = token.substr(0, pos);
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);  // key转小写
            std::string value = token.substr(pos + 1);
            if ("driver" == key)
            {
                // 去除左右的空格以及花括号
                std::string::size_type start = value.find_first_not_of(" \t{");
                std::string::size_type end = value.find_last_not_of(" \t}");
                result[key] = value.substr(start, end - start + 1);
            }
            else if ("dbq" == key)
            {
                // 解析oracle 的DBQ  IP:PORT/SCHEMA
                std::string ip_port_schema = value;
                pos = ip_port_schema.find('/');
                if (pos != std::string::npos)
                {
                    std::string schema = ip_port_schema.substr(pos + 1);
                    ip_port_schema = ip_port_schema.substr(0, pos);
                    pos = ip_port_schema.find(':');
                    if (pos != std::string::npos)
                    {
                        result["server"] = _trim(ip_port_schema.substr(0, pos));
                        result["port"] = _trim(ip_port_schema.substr(pos + 1));
                    }
                    else
                    {
                        result["server"] = _trim(ip_port_schema);
                    }
                    result["database"] = _trim(schema);
                }
            }
            else if ("tcp_port" == key)
            {
                result["port"] = _trim(value);
            }
            else if ("uid" == key)
            {
                result["user"] = _trim(value);
            }
            else if ("password" == key)
            {
                result["pwd"] = _trim(value);
            }
            else
            {
                result[key] = _trim(value);
            }
        }
    }

    return result;
}

bool otl::load(const std::string& cfg)
{
    clean();
    bool status = false;
    if (cfg.empty())
    {
        return status;
    }
    bool valid_json = false;
#if USE_JSON_PARSE
    valid_json = from_json(cfg);
#endif

    if (!valid_json)
    {
        std::map<std::string, std::string> k2v = parse_odbc(cfg);
        auto iter = k2v.find("dsn");
        if (iter != k2v.end())
        {
            m_dsn = iter->second;
        }
        else
        {
            iter = k2v.find("driver");
            if (iter == k2v.end())
            {
                m_err = "没有指定驱动";
                return status;
            }
            m_driver = iter->second;
            m_type = assume_type(m_driver);
            if (otl::eType::dbINVALID == m_type)
            {
                m_err = "无法识别的驱动: " + iter->second;
                return status;
            }

            if (otl::eType::dbKingB8 == m_type)
            {
                m_err = "金仓数据库未支持: " + iter->second;
                return status;
            }

            iter = k2v.find("port");
            if (iter != k2v.end())
            {
                m_port = std::stoi(iter->second);
            }
            else
            {
                // 使用默认端端口
                switch (m_type)
                {
                    case otl::eType::dbSQLSERVER:
                        m_port = 1433;
                        break;
                    case otl::eType::dbMYSQL:
                        m_port = 3306;
                        break;
                    case otl::eType::dbMariaDB:
                        m_port = 3306;
                        break;
                    case otl::eType::dbORACLE:
                        m_port = 1521;
                        break;
                    case otl::eType::dbPG:
                        m_port = 5432;
                        break;
                    case otl::eType::dbDM8:
                        m_port = 5236;
                        break;
                    default:
                        break;
                }
            }

            m_ip = k2v["server"];
            m_schema = k2v["database"];
        }
        // 检查用户名和密码
        iter = k2v.find("user");
        if (iter != k2v.end())
        {
            m_user = iter->second;
        }
        else
        {
            m_err = "没有指定用户名";
            return status;
        }

        iter = k2v.find("pwd");
        if (iter != k2v.end())
        {
            m_password = iter->second;
        }
        else
        {
            m_err = "没有指定密码";
            return status;
        }
    }
    if (m_dsn.empty())
    {
        // 检测驱动名称是否有效
        std::vector<std::string> driver = drivers();
        std::string lower_driver = _lower(m_driver);
        bool valid = false;
        for (auto& d : driver)
        {
            if (lower_driver == _lower(d))
            {
                if (d != m_driver)
                {
                    m_driver = d;  // 自动修正大小写
                }
                valid = true;
                break;
            }
        }

        if (!valid)
        {
            m_err = "无效的数据库驱动: " + m_driver;
            return status;
        }
    }
    m_conn = odbc(true);

    return check();
}

std::string otl::odbc(const bool& rebuild)
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
                case otl::eType::dbMYSQL:
                    sprintf(buff, SILLY_OTL_MYSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case otl::eType::dbSQLSERVER:
                    sprintf(buff, SILLY_OTL_MSSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_user.c_str(), m_password.c_str(), m_schema.c_str());
                    break;
                case otl::eType::dbORACLE:
                    sprintf(buff, SILLY_OTL_ORACLE_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case otl::eType::dbPG:
                    sprintf(buff, SILLY_OTL_POSTGRE_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case otl::eType::dbDM8:
                    sprintf(buff, SILLY_OTL_DM8_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_user.c_str(), m_password.c_str());
                    break;
                case otl::eType::dbMariaDB:
                    sprintf(buff, SILLY_OTL_MYSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                default:
                    break;
            }
            m_conn = buff;
        }
    }
    return m_conn;
}

bool otl::check()
{
    bool status = false;
    otl_connect db;
    try
    {
        db.set_timeout(m_timeout);
        db.rlogon(m_conn.c_str());
        status = true;
    }
    catch (otl_exception& e)
    {
        db.rollback();
        m_err = "OTL_ERR \nCONN:";
        m_err.append(m_conn);
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
    db.logoff();
    return status;
}

void otl::clean()
{
    m_ip.clear();
    m_port = 0;
    m_type = otl::eType::dbINVALID;
    m_driver.clear();
    m_schema.clear();
    m_user.clear();
    m_password.clear();
    m_dsn.clear();
    m_conn.clear();
    m_err.clear();
}

void otl::help()
{
    printf(
        "\nOTL 连接串帮助信息:\n >>> 账号和密码中不要出现 [ ] { } ( ) , ; ? * = ! @ | 这些特殊字符 <<<\nSQL Server:\n\tDRIVER={驱动名称};SERVER=IP;PORT=端口;UID=账号;PWD=密码;DATABASE=数据库;\nMySQL:\n\tDriver={MySQL ODBC 8.0 ANSI "
        "Driver};Server=IP;Port=端口;Database=数据库;User=账号;Password=密码;Option=3;charset=UTF8;\nOracle:\n\tDriver={ODBC驱动名称};DBQ=IP:端口/表空间名称;UID=用户;PWD=密码;Oracle需要另外设置环境变量NLS_LANG=SIMPLIFIED "
        "CHINESE_CHINA.UTF8,以支持中文编码utf8传递;\n达梦(DM8):\n\tDriver={驱动名称};Server=IP;TCP_PORT:端口;UID=账号;PWD=密码; \n\t即使数据库编码为UTF8, 数据在插入时也需要时GBK编码, 否则会乱码;"
        "\n不能正常使用ODBC时,考虑使用DSN方式:\n\tUID=账号;PWD=密码;DSN=DNS名称;\n");
    std::string content = "\n\n当前机器支持的ODBC驱动:\n";
    for (auto d : drivers())
    {
        content += d + "\n";
    }

    printf(content.c_str());
}

static char* sqlserver_code_sql = "SELECT COLLATIONPROPERTY('Chinese_PRC_Stroke_CI_AI_KS_WS', 'CodePage');";
static std::map<std::string, std::string> sqlserver_code_map = {{"936", "GBK"}, {"950", "BIG5"}, {"437", "Eng"}, {"932", "JP"}, {"949", "KOREA"}, {"866", "RUSSIA"}, {"65001", "UFT-8"}, {"", "INVALID"}};

std::string otl::encode()
{
    std::string result;
    std::string code;
    otl_connect db;
    try
    {
        m_conn = odbc(true);
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
        db.rollback();
        m_err = "OTL_ERR \nCONN:";
        m_err.append(m_conn);
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
    db.logoff();
    result = sqlserver_code_map[code];
    return result;
}

#ifdef IS_WIN32
#include <odbcinst.h>
#include <cstring>
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif
std::vector<std::string> otl::drivers()
{
    std::vector<std::string> ret;
#ifdef IS_WIN32
    std::vector<WCHAR> szBuf(10240);
    WORD cbBufMax = 10239;
    WORD cbBufOut;
    WCHAR* pszBuf = szBuf.data();
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    if (SQLGetInstalledDrivers(szBuf.data(), cbBufMax, &cbBufOut))
    {
        do
        {
            pszBuf = wcschr(pszBuf, '\0') + 1;
            ret.push_back(converterX.to_bytes(pszBuf));
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
        tmp_odbc_driver = tmp_odbc_driver.substr(1, tmp_odbc_driver.size() - 3);  // 每一行的结果 [MySQL ODBC 8.0 Unicode Driver]\r    最后有个换行符,所以是 -3
        ret.push_back(tmp_odbc_driver);
        memset(buffer, 0, 4096);
    }

    pclose(fp);
#endif
    return ret;
}
bool otl::check_column_info(const std::string& sql)
{
    bool status = false;
    otl_connect db;
    try
    {
        db.auto_commit_off();
        db.set_timeout(m_timeout);
        db.rlogon(m_conn.c_str(), false);
        otl_stream stream;
        stream.open(1, sql.c_str(), db);

        int col_num = 0;
        otl_column_desc* desc_list = stream.describe_select(col_num);
        std::cout << "列数: " << col_num << std::endl;
        for (int i = 0; i < col_num; ++i)
        {
            std::cout << "[" << i + 1 << "] " << "列名: " << desc_list[i].name << "  类型: " << otl_type_name((otl_var_enum)desc_list[i].otl_var_dbtype) << std::endl;
        }
        stream.close();
        status = true;
    }
    catch (otl_exception& e)
    {
        db.rollback();
        m_err = "OTL_ERR \nCONN:";
        m_err.append(m_conn);
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
    db.logoff();

    return status;
}

std::string otl::otl_type_name(const otl_var_enum& ot)
{
    std::string result = "Unknown";
    switch (ot)
    {
        case otl_var_char:
            result = "char";
            break;
        case otl_var_double:
            result = "double";
            break;
        case otl_var_float:
            result = "float";
            break;
        case otl_var_int:
            result = "int ";
            break;
        case otl_var_unsigned_int:
            result = "unsigned_int";
            break;
        case otl_var_short:
            result = "short";
            break;
        case otl_var_long_int:
            result = "long_int";
            break;
        case otl_var_timestamp:
            result = "timestamp";
            break;
        case otl_var_varchar_long:
            result = "varchar_long";
            break;
        case otl_var_raw_long:
            result = "raw_long";
            break;
        case otl_var_clob:
            result = "clob";
            break;
        case otl_var_blob:
            result = "blob";
            break;
        case otl_var_refcur:
            result = "refcur";
            break;
        case otl_var_long_string:
            result = "long_string";
            break;
        case otl_var_db2time:
            result = "db2time";
            break;
        case otl_var_db2date:
            result = "db2date";
            break;
        case otl_var_tz_timestamp:
            result = "tz_timestamp";
            break;
        case otl_var_ltz_timestamp:
            result = "ltz_timestamp";
            break;
        case otl_var_bigint:
            result = "bigint";
            break;
        case otl_var_raw:
            result = "raw";
            break;
        default:
            result = "unknown";
            break;
    }
    return result;
}

#if USE_JSON_PARSE

bool silly::db::otl::from_json(const std::string& jstr)
{
    Json::Value root = silly_jsonpp::loads(jstr);
    if (root.isNull())
    {
        return false;
    }

    return from_json(root);
}

bool silly::db::otl::from_json(const Json::Value& root)
{
    bool status = false;
    silly_jsonpp::check_str(root, OPT_STR_DSN, m_dsn);
    if (m_dsn.empty())  // 非DSN方式
    {
        // 检查类型
        std::string type_str;
        if (!silly_jsonpp::check_str(root, OPT_STR_TYPE, type_str))
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

        if (!silly_jsonpp::check_str(root, OPT_STR_IP, m_ip))
        {
            m_err = "未指定IP";
            return status;
        }
        if (!silly_jsonpp::check_str(root, OPT_STR_DRIVER, m_driver))
        {
            m_err = "未指定驱动";
            return status;
        }

        // 端口
        if (root.isMember(OPT_STR_PORT))
        {
            if (root[OPT_STR_PORT].isInt())
            {
                m_port = root[OPT_STR_PORT].asInt();
            }
            else if (root[OPT_STR_PORT].isString())
            {
                m_port = std::stoi(root[OPT_STR_PORT].asString());
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

        if (!silly_jsonpp::check_str(root, OPT_STR_SCHEMA, m_schema) && (enum_database_type::dbDM8 != m_type))
        {
            m_err = "未指定数据库";
            return status;
        }
    }
    if (!silly_jsonpp::check_str(root, OPT_STR_USER, m_user))
    {
        m_err = "未指定用户名";
        return status;
    }
    if (!silly_jsonpp::check_str(root, OPT_STR_PASSWORD, m_password))
    {
        m_err = "未指定密码";
        return status;
    }
    silly_jsonpp::check_bool(root, OPT_STR_VERBOSE, m_verbose);

    return true;
}
#endif

void otl::verbose(bool vb)
{
    m_verbose = vb;
}

otl::eType otl::type() const
{
    return m_type;
}

std::string otl::driver() const
{
    return m_driver;
}

std::string otl::ip() const
{
    return m_ip;
}

int otl::port() const
{
    return m_port;
}

std::string otl::schema() const
{
    return m_schema;
}

std::string otl::user() const
{
    return m_user;
}

std::string otl::pwd() const
{
    return m_password;
}

std::string otl::err() const
{
    return m_err;
}

void otl::type(otl::eType tp)
{
    m_type = tp;
}

void otl::driver(std::string d)
{
    m_driver = d;
}

void otl::ip(std::string i)
{
    m_ip = i;
}

void otl::port(int p)
{
    m_port = p;
}

void otl::schema(std::string s)
{
    m_schema = s;
}

void otl::user(std::string u)
{
    m_user = u;
}

void otl::pwd(std::string p)
{
    m_password = p;
}

void otl::timeout(int to)
{
    m_timeout = to;
}
otl::eType otl::str2type(const std::string& desc)
{
    if (TYPE_MSSQL_STR == desc)
    {
        return eType::dbSQLSERVER;
    }
    else if (TYPE_MYSQL_STR == desc)
    {
        return eType::dbMYSQL;
    }
    else if (TYPE_ORACLE_STR == desc)
    {
        return eType::dbORACLE;
    }
    else if (TYPE_DM8_STR == desc)
    {
        return eType::dbDM8;
    }
    else if (TYPE_POSTGRESQL_STR == desc)
    {
        return eType::dbPG;
    }
    else if (TYPE_KING8_STR == desc)
    {
        return eType::dbKingB8;
    }
    else if (TYPE_MARIA_STR == desc)
    {
        return eType::dbMariaDB;
    }
    return eType::dbINVALID;
}

std::string otl::type2str(const otl::eType& type)
{
    std::string s_ret;
    switch (type)
    {
        case eType::dbSQLSERVER:
            s_ret = TYPE_MSSQL_STR;
            break;
        case eType::dbMYSQL:
            s_ret = TYPE_MYSQL_STR;
            break;
        case eType::dbORACLE:
            s_ret = TYPE_ORACLE_STR;
            break;
        case eType::dbDM8:
            s_ret = TYPE_DM8_STR;
            break;
        case eType::dbPG:
            s_ret = TYPE_POSTGRESQL_STR;
            break;
        case eType::dbKingB8:
            s_ret = TYPE_KING8_STR;
            break;
        case eType::dbMariaDB:
            s_ret = TYPE_MARIA_STR;
            break;
        default:
            s_ret = "";
            break;
    }
    return s_ret;
}
