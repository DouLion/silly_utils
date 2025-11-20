//
// Created by dell on 2023/12/21.
//
#pragma once
#include "silly_otl.h"

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

/// <summary>
/// 根据驱动名称,猜测数据库类型
/// </summary>
/// <param name="driver"></param>
/// <returns></returns>
static eOtlDbType assume_type(const std::string& driver)
{
    std::string lower_driver = TO_LOWER(driver);
    if (lower_driver.find("sql server") != std::string::npos)
    {
        return eOtlDbType::dbSQLSERVER;
    }
    if (lower_driver.find("mysql") != std::string::npos)
    {
        return eOtlDbType::dbMYSQL;
    }
    if (lower_driver.find("oracle") != std::string::npos)
    {
        return eOtlDbType::dbORACLE;
    }
    if (lower_driver.find("postgresql") != std::string::npos)
    {
        return eOtlDbType::dbPG;
    }
    if (lower_driver.find("dm8") != std::string::npos)
    {
        return eOtlDbType::dbDM8;
    }
    if (lower_driver.find("maria") != std::string::npos)
    {
        return eOtlDbType::dbKingB8;
    }

    return eOtlDbType::dbINVALID;
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
                        result["server"] = TRIM(ip_port_schema.substr(0, pos));
                        result["port"] = TRIM(ip_port_schema.substr(pos + 1));
                    }
                    else
                    {
                        result["server"] = TRIM(ip_port_schema);
                    }
                    result["database"] = TRIM(schema);
                }
            }
            else if ("tcp_port" == key)
            {
                result["port"] = TRIM(value);
            }
            else if ("uid" == key)
            {
                result["user"] = TRIM(value);
            }
            else if ("password" == key)
            {
                result["pwd"] = TRIM(value);
            }
            else
            {
                result[key] = TRIM(value);
            }
        }
    }

    return result;
}


std::string suOTL::LStr2Str(const otl_long_string& lstr)
{
    std::string ret;
    ret.resize(lstr.len());
    std::memcpy(&ret[0], lstr.v, lstr.len());
    return ret;
}
std::string suOTL::Lob2Str(otl_lob_stream* stream)
{
    std::string ret;
    while (!stream->eof())
    {
        std::string tmp;
        otl_long_string _sols;
        *stream >> _sols;
        tmp.resize(_sols.len());
        std::memcpy(&tmp[0], _sols.v, _sols.len());
        ret += tmp;
    }
    stream->close();

    return ret;
}
std::string suOTL::Time2Str(const otl_datetime& dt, const bool& millisecond)
{
    char buff[32];
    if (millisecond)
    {
        SU_SPRINTF(buff, "%04d-%02d-%02d %02d:%02d:%02d.%03d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, int(dt.fraction / 1e6));
    }
    else
    {
        SU_SPRINTF(buff, "%04d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    }
    return buff;
}
otl_datetime suOTL::Str2Time(const std::string& str)
{
    otl_datetime dt{};
    // 正则：年(4位) - 月(1~2) - 日(1~2) [空格 时(1~2) : 分(1~2) [: 秒(1~2) [ . 毫秒(1~3) ]]]
    const std::regex pattern(
        R"((\d{4})-(\d{1,2})-(\d{1,2})(?: (\d{1,2}):(\d{1,2})(?::(\d{1,2})(?:\.(\d{1,3}))?)?)?)"
    );

    std::smatch matches;

    if (std::regex_search(str, matches, pattern)) {
        // ===== 必选部分：年、月、日 =====
        dt.year  = std::stoi(matches[1]);
        dt.month = std::stoi(matches[2]);
        dt.day   = std::stoi(matches[3]);

        // ===== 可选部分：时:分:秒.毫秒 =====
        if (matches.size() > 4 && matches[4].matched) {  // 有时:分
            dt.hour   = std::stoi(matches[4]);
            dt.minute = std::stoi(matches[5]);

            if (matches.size() > 6 && matches[6].matched) {  // 有秒
                dt.second = std::stoi(matches[6]);

                if (matches.size() > 7 && matches[7].matched) {  // 有毫秒
                    std::string ms_str = matches[7];
                    int ms_len = ms_str.length();

                    dt.fraction = ms_str.empty() ? 0 : static_cast<unsigned long>(std::stoul(ms_str));

                    // 可选：记录毫秒精度（1~3位）
                    dt.frac_precision = ms_len;  // 1, 2, 或 3

                    // 如果你希望 fraction 是微秒，可以乘以 10^(3-ms_len)
                    // 例如 .1 → 100, .12 → 120, .123 → 123
                    // 但目前我们直接存原始值，你可以后续按需处理
                } else {
                    dt.fraction = 0;
                    dt.frac_precision = 0;
                }
            } else {
                dt.second = 0;
                dt.fraction = 0;
                dt.frac_precision = 0;
            }
        } else {
            // 只有年月日
            dt.hour = 0;
            dt.minute = 0;
            dt.second = 0;
            dt.fraction = 0;
            dt.frac_precision = 0;
        }
    }

    return dt;
}
std::time_t suOTL::Time2Stamp(const otl_datetime& dt)
{
    std::tm stm;
    stm.tm_year = dt.year - 1900;
    stm.tm_mon = dt.month - 1;
    stm.tm_mday = dt.day;
    stm.tm_hour = dt.hour;
    stm.tm_min = dt.minute;
    stm.tm_sec = dt.second;
    return std::mktime(&stm);
}
otl_datetime suOTL::Stamp2Time(const std::time_t& stamp)
{
    std::tm* stm = std::localtime(&stamp);
    otl_datetime dt;
    dt.year = stm->tm_year + 1900;
    dt.month = stm->tm_mon + 1;
    dt.day = stm->tm_mday;
    dt.hour = stm->tm_hour;
    dt.minute = stm->tm_min;
    dt.second = stm->tm_sec;
    dt.fraction = 0;
    return dt;
}
bool suOTL::load(const std::string& cfg)
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
            if (eOtlDbType::dbINVALID == m_type)
            {
                m_err = "无法识别的驱动: " + iter->second;
                return status;
            }

            if (eOtlDbType::dbKingB8 == m_type)
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
                    case eOtlDbType::dbSQLSERVER:
                        m_port = 1433;
                        break;
                    case eOtlDbType::dbMYSQL:
                        m_port = 3306;
                        break;
                    case eOtlDbType::dbMariaDB:
                        m_port = 3306;
                        break;
                    case eOtlDbType::dbORACLE:
                        m_port = 1521;
                        break;
                    case eOtlDbType::dbPG:
                        m_port = 5432;
                        break;
                    case eOtlDbType::dbDM8:
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
        std::string lower_driver = TO_LOWER(m_driver);
        bool valid = false;
        for (auto& d : driver)
        {
            if (lower_driver == TO_LOWER(d))
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
suOTL& suOTL::operator=(const suOTL& rh)
{
    this->m_ip = rh.m_ip;
    this->m_port = rh.m_port;
    this->m_type = rh.m_type;
    this->m_driver = rh.m_driver;
    this->m_schema = rh.m_schema;
    this->m_user = rh.m_user;
    this->m_password = rh.m_password;
    this->m_dsn = rh.m_dsn;
    this->m_timeout = rh.m_timeout;
    this->m_conn = rh.m_conn;
    this->m_verbose = rh.m_verbose;

    return *this;
}

std::string suOTL::odbc(const bool& rebuild)
{
    if (m_conn.empty() || rebuild)
    {
        char buff[SILLY_OTL_ODBC_MAX_LEN] = {0};
        if (!m_dsn.empty())  // 如果ODBC连接串不好使,设置DSN,并且优先使用DSN链接方式
        {
            SU_SPRINTF(buff, SILLY_OTL_DSN_FORMAT.c_str(), m_user.c_str(), m_password.c_str(), m_dsn.c_str());
            m_conn = buff;
        }
        else if (!m_driver.empty())
        {
            switch (m_type)  // 使用ODBC连接串
            {
                case eOtlDbType::dbMYSQL:
                    SU_SPRINTF(buff, SILLY_OTL_MYSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case eOtlDbType::dbSQLSERVER:
                    SU_SPRINTF(buff, SILLY_OTL_MSSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_user.c_str(), m_password.c_str(), m_schema.c_str());
                    break;
                case eOtlDbType::dbORACLE:
                    SU_SPRINTF(buff, SILLY_OTL_ORACLE_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case eOtlDbType::dbPG:
                    SU_SPRINTF(buff, SILLY_OTL_POSTGRE_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                case eOtlDbType::dbDM8:
                    SU_SPRINTF(buff, SILLY_OTL_DM8_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_user.c_str(), m_password.c_str());
                    break;
                case eOtlDbType::dbMariaDB:
                    SU_SPRINTF(buff, SILLY_OTL_MYSQL_ODBC_FORMAT.c_str(), m_driver.c_str(), m_ip.c_str(), m_port, m_schema.c_str(), m_user.c_str(), m_password.c_str());
                    break;
                default:
                    break;
            }
            m_conn = buff;
        }
    }
    return m_conn;
}

bool suOTL::check(const bool& rebuild_odbc)
{
    bool status = false;
    if (rebuild_odbc)
    {
        odbc(true);
    }
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
        m_err.append("\nMSG:").append(std::string(reinterpret_cast<char*>(e.msg)));
        m_err.append("\nSTATE:").append(std::string(reinterpret_cast<char*>(e.sqlstate)));
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

void suOTL::clean()
{
    m_ip.clear();
    m_port = 0;
    m_type = eOtlDbType::dbINVALID;
    m_driver.clear();
    m_schema.clear();
    m_user.clear();
    m_password.clear();
    m_dsn.clear();
    m_conn.clear();
    m_err.clear();
}

void suOTL::help()
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

    std::cout << content;
}

static const char* sqlserver_code_sql = "SELECT COLLATIONPROPERTY('Chinese_PRC_Stroke_CI_AI_KS_WS', 'CodePage');";
static std::map<std::string, std::string> sqlserver_code_map = {{"936", "GBK"}, {"950", "BIG5"}, {"437", "Eng"}, {"932", "JP"}, {"949", "KOREA"}, {"866", "RUSSIA"}, {"65001", "UFT-8"}, {"", "INVALID"}};

std::string suOTL::encode()
{
    std::string result;
    std::string code;
    otl_connect db;
    try
    {
        m_conn = odbc(true);
        db.rlogon(m_conn.c_str());
        char buff[512] = {0};
        SU_SPRINTF(buff, "%s", sqlserver_code_sql);
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
        make_otl_err(e);
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
std::vector<std::string> suOTL::drivers()
{
    std::vector<std::string> ret;
#ifdef IS_WIN32
    std::vector<WCHAR> szBuf(10240);
    WORD cbBufMax = 10239;
    WORD cbBufOut;
    WCHAR* pszBuf = szBuf.data();
    if (SQLGetInstalledDrivers(szBuf.data(), cbBufMax, &cbBufOut))
    {
        do
        {
            pszBuf = wcschr(pszBuf, '\0') + 1;
            ret.push_back(WS2S(pszBuf));
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
bool suOTL::check_column_info(const std::string& sql)
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
            std::cout << "[" << i + 1 << "] "
                      << "列名: " << desc_list[i].name << "  类型: " << otl_type_name(static_cast<otl_var_enum>(desc_list[i].otl_var_dbtype)) << std::endl;
        }
        stream.close();
        status = true;
    }
    catch (otl_exception& e)
    {
        db.rollback();
        make_otl_err(e);
    }
    catch (std::exception& p)
    {
        db.rollback();
        m_err = "OTL_UNKNOWN " + std::string(p.what());
    }
    db.logoff();

    return status;
}

std::string suOTL::otl_type_name(const otl_var_enum& vt)
{
    switch (vt)
    {
        case otl_var_char:
            return "char";
        case otl_var_double:
            return "double";
        case otl_var_float:
            return "float";
        case otl_var_int:
            return "int ";
        case otl_var_unsigned_int:
            return "unsigned_int";

        case otl_var_short:
            return "short";

        case otl_var_long_int:
            return "long_int";

        case otl_var_timestamp:
            return "timestamp";

        case otl_var_varchar_long:
            return "varchar_long";

        case otl_var_raw_long:
            return "raw_long";

        case otl_var_clob:
            return "clob";

        case otl_var_blob:
            return "blob";

        case otl_var_refcur:
            return "refcur";

        case otl_var_long_string:
            return "long_string";

        case otl_var_db2time:
            return "db2time";

        case otl_var_db2date:
            return "db2date";

        case otl_var_tz_timestamp:
            return "tz_timestamp";

        case otl_var_ltz_timestamp:
            return "ltz_timestamp";

        case otl_var_bigint:
            return "bigint";

        case otl_var_raw:
            return "raw";

        default:
            break;
    }
    return "unknown";
}

#if USE_JSON_PARSE

bool suOTL::from_json(const std::string& jstr)
{
    Json::Value root = silly_jsonpp::loads(jstr);
    if (root.isNull())
    {
        return false;
    }

    return from_json(root);
}

bool suOTL::from_json(const Json::Value& root)
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
        if (eOtlDbType::dbINVALID == m_type)
        {
            m_err = SUFMT("不支持的数据库类型 (Unsupported database type): {}.", type_str);
            return status;
        }
        if (eOtlDbType::dbKingB8 == m_type)
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
                case eOtlDbType::dbSQLSERVER:
                    m_port = 1433;
                    break;
                case eOtlDbType::dbMYSQL:
                    m_port = 3306;
                    break;
                case eOtlDbType::dbORACLE:
                    m_port = 1521;
                    break;
                case eOtlDbType::dbPG:
                    m_port = 5432;
                    break;
                case eOtlDbType::dbDM8:
                    m_port = 5236;
                    break;
                default:
                    break;
            }
        }

        if (!silly_jsonpp::check_str(root, OPT_STR_SCHEMA, m_schema) && (eOtlDbType::dbDM8 != m_type))
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
    silly_jsonpp::check_int32(root, "timeout", m_timeout);

    return true;
}
#endif

void suOTL::verbose(const bool& vb)
{
    m_verbose = vb;
}

eOtlDbType suOTL::type() const
{
    return m_type;
}

std::string suOTL::driver() const
{
    return m_driver;
}

std::string suOTL::ip() const
{
    return m_ip;
}

int suOTL::port() const
{
    return m_port;
}

std::string suOTL::schema() const
{
    return m_schema;
}

std::string suOTL::user() const
{
    return m_user;
}

std::string suOTL::pwd() const
{
    return m_password;
}

std::string suOTL::err() const
{
    return m_err;
}

void suOTL::type(const eOtlDbType& tp)
{
    m_type = tp;
}

void suOTL::driver(const std::string& d)
{
    m_driver = d;
}

void suOTL::ip(const std::string& i)
{
    m_ip = i;
}

void suOTL::port(const int& p)
{
    m_port = p;
}

void suOTL::schema(const std::string& s)
{
    m_schema = s;
}

void suOTL::user(const std::string& u)
{
    m_user = u;
}

void suOTL::pwd(const std::string& p)
{
    m_password = p;
}

void suOTL::timeout(const int& to)
{
    m_timeout = to;
}
eOtlDbType suOTL::str2type(const std::string& desc)
{
    if (TYPE_MSSQL_STR == desc)
    {
        return eOtlDbType::dbSQLSERVER;
    }
    else if (TYPE_MYSQL_STR == desc)
    {
        return eOtlDbType::dbMYSQL;
    }
    else if (TYPE_ORACLE_STR == desc)
    {
        return eOtlDbType::dbORACLE;
    }
    else if (TYPE_DM8_STR == desc)
    {
        return eOtlDbType::dbDM8;
    }
    else if (TYPE_POSTGRESQL_STR == desc)
    {
        return eOtlDbType::dbPG;
    }
    else if (TYPE_KING8_STR == desc)
    {
        return eOtlDbType::dbKingB8;
    }
    else if (TYPE_MARIA_STR == desc)
    {
        return eOtlDbType::dbMariaDB;
    }
    return eOtlDbType::dbINVALID;
}

std::string suOTL::type2str(const eOtlDbType& type)
{
    std::string s_ret;
    switch (type)
    {
        case eOtlDbType::dbSQLSERVER:
            s_ret = TYPE_MSSQL_STR;
            break;
        case eOtlDbType::dbMYSQL:
            s_ret = TYPE_MYSQL_STR;
            break;
        case eOtlDbType::dbORACLE:
            s_ret = TYPE_ORACLE_STR;
            break;
        case eOtlDbType::dbDM8:
            s_ret = TYPE_DM8_STR;
            break;
        case eOtlDbType::dbPG:
            s_ret = TYPE_POSTGRESQL_STR;
            break;
        case eOtlDbType::dbKingB8:
            s_ret = TYPE_KING8_STR;
            break;
        case eOtlDbType::dbMariaDB:
            s_ret = TYPE_MARIA_STR;
            break;
        default:
            s_ret = "";
            break;
    }
    return s_ret;
}

std::string suOTL::last_insert_id_sql() const
{
    if (eOtlDbType::dbMYSQL == m_type)
    {
        return silly::sql::mysql::LAST_INSERT_ID;
    }
    else if (eOtlDbType::dbSQLSERVER == m_type)
    {
        return silly::sql::sqlserver::LAST_INSERT_ID;
    }
    else if (eOtlDbType::dbORACLE == m_type)
    {
        return silly::sql::oracle::LAST_INSERT_ID;
    }
    else if (eOtlDbType::dbPG == m_type)
    {
        return silly::sql::postgresql::LAST_INSERT_ID;
    }
    else if (eOtlDbType::dbDM8 == m_type)
    {
        return silly::sql::dm8::LAST_INSERT_ID;
    }
    else if (eOtlDbType::dbKingB8 == m_type)
    {
        return silly::sql::kingb8::LAST_INSERT_ID;
    }
    return "";
}
