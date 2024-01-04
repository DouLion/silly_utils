/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/21 13:42
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_OTL_H
#define SILLY_UTILS_SILLY_OTL_H

#define OTL_ODBC

#ifndef IS_WIN32
#define OTL_ODBC_UNIX
#define OTL_ODBC_MYSQL
#else

#endif

#define OTL_STREAM_READ_ITERATOR_ON  // image 等大量数据插入的支持
#define OTL_STL                      // 支持用std::string 接受char* 类型

#define OTL_ANSI_CPP                        // 中文支持
#define OTL_ANSI_CPP_11_VARIADIC_TEMPLATES  // 支持 otl_value< >
#define OTL_CPP_11_ON                       // 支持连接池的 c++ 版本
#define OTL_CONNECT_POOL_ON                 // 连接池支持

#ifndef NOMINMAX  // std::max std::min  c++ 17 版本的一些问题
#define NOMINMAX
#endif

#ifndef _CRT_SECURE_NO_WARNINGS  // 函数安全
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <database/otl/otlv4.h>

#ifdef IS_WIN32
#pragma comment(lib, "odbc32.lib")
#endif

enum enum_database_type
{
    dbINVALID = 0,    // 无效数据源类型
    dbSQLSERVER = 1,  // SQLSERVER 数据库
    dbMYSQL = 2,      // MYSQL 数据库
    dbORACLE = 3,     // ORACLE 数据库
    dbDM8 = 4,        // 达梦
    dbPG = 5,         // postgres
    dbKingB8 = 6      // 人大金仓
};

const static char* SILLY_DB_TYPE_MSSQL_STR = "sqlserver";
const static char* SILLY_DB_TYPE_MYSQL_STR = "mysql";
const static char* SILLY_DB_TYPE_ORACLE_STR = "oracle";
const static char* SILLY_DB_TYPE_DM8_STR = "dm8";
const static char* SILLY_DB_TYPE_POSTGRESQL_STR = "postgresql";
const static char* SILLY_DB_TYPE_KINGB8_STR = "kb8";

static enum_database_type str_to_db_type(const std::string& desc)
{
    if (SILLY_DB_TYPE_MSSQL_STR == desc)
    {
        return enum_database_type::dbSQLSERVER;
    }
    else if (SILLY_DB_TYPE_MYSQL_STR == desc)
    {
        return enum_database_type::dbMYSQL;
    }
    else if (SILLY_DB_TYPE_ORACLE_STR == desc)
    {
        return enum_database_type::dbORACLE;
    }
    else if (SILLY_DB_TYPE_DM8_STR == desc)
    {
        return enum_database_type::dbDM8;
    }
    else if (SILLY_DB_TYPE_POSTGRESQL_STR == desc)
    {
        return enum_database_type::dbPG;
    }
    else if (SILLY_DB_TYPE_KINGB8_STR == desc)
    {
        return enum_database_type::dbKingB8;
    }
    return enum_database_type::dbINVALID;
}

static std::string db_type_to_str(const enum_database_type& type)
{
    std::string s_ret;
    switch (type)
    {
        case dbSQLSERVER:
            s_ret = SILLY_DB_TYPE_MSSQL_STR;
            break;
        case dbMYSQL:
            s_ret = SILLY_DB_TYPE_MYSQL_STR;
            break;
        case dbORACLE:
            s_ret = SILLY_DB_TYPE_ORACLE_STR;
            break;
        case dbDM8:
            s_ret = SILLY_DB_TYPE_DM8_STR;
            break;
        case dbPG:
            s_ret = SILLY_DB_TYPE_POSTGRESQL_STR;
            break;
        case dbKingB8:
            s_ret = SILLY_DB_TYPE_KINGB8_STR;
            break;
        default:
            s_ret = "";
            break;
    }
    return s_ret;
}

#define SILLY_OTL_OPT_S_IP "ip"
#define SILLY_OTL_OPT_S_PORT "port"
#define SILLY_OTL_OPT_S_TYPE "type"
#define SILLY_OTL_OPT_S_DRIVER "driver"
#define SILLY_OTL_OPT_S_SCHEMA "schema"
#define SILLY_OTL_OPT_S_USER "user"
#define SILLY_OTL_OPT_S_PASSWORD "password"
#define SILLY_OTL_OPT_S_DSN "dsn"

#define SILLY_OLT_LOB_STREAM_TO_STRING(lob, str) \
    while (!lob.eof())                           \
    {                                            \
        std::string tmp;                         \
        otl_long_string _sols;                   \
        lob >> _sols;                            \
        tmp.resize(_sols.len());                 \
        memcpy(&tmp[0], _sols.v, _sols.len());   \
        str += tmp;                              \
    }                                            \
    lob.close();

class otl_conn_opt
{
  public:
    bool load(const std::string& s_opt);

    std::string dump_odbc(const bool& rebuild = false);
    void help();

  public:
    std::string ip;
    int port;
    enum_database_type type;
    std::string driver;
    std::string schema;
    std::string user;
    std::string password;
    std::string dsn;

  protected:
    std::string conn;
};

#endif  // SILLY_UTILS_SILLY_OTL_H
