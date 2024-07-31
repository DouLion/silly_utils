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
#include <log/silly_log.h>
#include <functional>
#include <stdexcept>  // 包含标准异常类

#ifdef IS_WIN32
#pragma comment(lib, "odbc32.lib")
#endif

#define SILLY_OTL_ODBC_MAX_LEN 1024

enum class enum_database_type
{
    dbINVALID = 0,    // 无效数据源类型
    dbSQLSERVER = 1,  // SQLSERVER 数据库
    dbMYSQL = 2,      // MYSQL 数据库
    dbORACLE = 3,     // ORACLE 数据库
    dbDM8 = 4,        // 达梦
    dbPG = 5,         // postgres
    dbKingB8 = 6      // 人大金仓
};

constexpr char* SILLY_DB_TYPE_MSSQL_STR = "sqlserver";
constexpr char* SILLY_DB_TYPE_MYSQL_STR = "mysql";
constexpr char* SILLY_DB_TYPE_ORACLE_STR = "oracle";
constexpr char* SILLY_DB_TYPE_DM8_STR = "dm8";
constexpr char* SILLY_DB_TYPE_POSTGRESQL_STR = "postgresql";
constexpr char* SILLY_DB_TYPE_KINGB8_STR = "kb8";

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
        case enum_database_type::dbSQLSERVER:
            s_ret = SILLY_DB_TYPE_MSSQL_STR;
            break;
        case enum_database_type::dbMYSQL:
            s_ret = SILLY_DB_TYPE_MYSQL_STR;
            break;
        case enum_database_type::dbORACLE:
            s_ret = SILLY_DB_TYPE_ORACLE_STR;
            break;
        case enum_database_type::dbDM8:
            s_ret = SILLY_DB_TYPE_DM8_STR;
            break;
        case enum_database_type::dbPG:
            s_ret = SILLY_DB_TYPE_POSTGRESQL_STR;
            break;
        case enum_database_type::dbKingB8:
            s_ret = SILLY_DB_TYPE_KINGB8_STR;
            break;
        default:
            s_ret = "";
            break;
    }
    return s_ret;
}

constexpr char* SILLY_OTL_MYSQL_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;Database=%s;User=%s;Password=%s;Option=3;charset=UTF8;";
constexpr char* SILLY_OTL_MSSQL_ODBC_FORMAT = "Driver={%s};Server=%s;Port:%d;Database=%s;UID=%s;PWD=%s;";
constexpr char* SILLY_OTL_ORACLE_ODBC_FORMAT = "Driver={%s};DBQ=%s:%d/%s;Uid=%s;Pwd=%s;";
constexpr char* SILLY_OTL_POSTGRE_ODBC_FORMAT = "Driver={%s};Server=%s;Port=%d;Database=%s;Uid=%s;Pwd=%s;";
constexpr char* SILLY_OTL_DSN_FORMAT = "UID=%s;PWD=%s;DSN=%s;";

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

class otl_tools;
class otl_conn_opt
{
    friend class otl_tools;

  public:
    /// <summary>
    /// 从字符串加载otl连接属性
    /// </summary>
    /// <param name="s_opt"></param>
    /// <returns></returns>
    bool load(const std::string& s_opt);

    /// <summary>
    /// 获取odbc连接串
    /// </summary>
    /// <param name="rebuild"></param>
    /// <returns></returns>
    std::string dump_odbc(const bool& rebuild = false);

    enum_database_type get_type();

    std::string get_schema();
    std::string get_ip();
    std::string get_user();
    std::string get_pwd();
    void set_user(const std::string& user);
    void set_pwd(const std::string& pwd);
    std::string get_driver();
    int get_port();

    /// <summary>
    /// 检查是否能够正常联通
    /// </summary>
    /// <returns></returns>
    bool check();

    /// <summary>
    /// 清空所有信息
    /// </summary>
    void clean();

    /// <summary>
    /// 打印提示信息
    /// </summary>
    void help();

    std::string encode();

  protected:
    std::string m_ip;
    int m_port{0};
    enum_database_type m_type{enum_database_type::dbINVALID};
    std::string m_driver;
    std::string m_schema;
    std::string m_user;
    std::string m_password;
    std::string m_dsn;
    std::string conn;
};

class silly_otl_connect
{
  public:
    template <typename Func, typename... Args>
    typename std::result_of<Func(Args...)>::type try_catch_otl(otl_conn_opt& oco, Func&& func, Args&&... args)
    {
        otl_connect db;
        const char* conn = oco.dump_odbc().c_str();
        try
        {
            db.rlogon(conn, false);
            db.auto_commit_off();
            db.set_timeout(10);
            db.set_max_long_size(INT_MAX - 1);
            return func(std::forward<Args>(args)...);
        }
        catch (otl_exception& e)
        {
            db.rollback();
            //SLOG_F(ERROR, "\n{}{}\n数据库查询失败, \nCONN:{} \nCODE:{} \nMSG:{} \nSTATE:{}\n", __FILE__, __LINE__, conn, std::string(e.code), std::string(e.msg), std::string(e.sqlstate));
            //throw;  // 如果需要，可以在这里重新抛出异常
        }
        catch (std::exception& p)
        {
            db.rollback();
            //SLOG_F(ERROR, "\n{}{}未知异常{}\n", __FILE__, __LINE__, std::string(p.what()));
            //throw;  // 如果需要，可以在这里重新抛出异常
        }
        //finally
        {
            db.logoff();
        }
    }
};

//
//void otl_logon(otl_connect& db, const char* conn, const bool& autocommit)
//{
//    db.rlogon(conn, autocommit);
//}
//
//template <typename Func, typename Obj, typename... Args>
//auto otl_exception_wrapper1(Func&& func, Obj&& obj, std::string&& err, Args&&... args)
//    -> decltype(std::invoke(std::forward<Func>(func), std::forward<Obj>(obj), std::forward<Args>(args)...))
//{
//    try
//    {
//        auto result = std::invoke(std::forward<Func>(func), std::forward<Obj>(obj), std::forward<Args>(args)...);
//    }
//    catch (otl_exception& e)
//    {
//#ifdef ENABLE_SILLY_LOG
//        LOG_F(ERROR, "OTL异常, \nSQL:\n\t%s \nMSG:\n\t%s \nSTATE:\n\t%s \nCODE:\n\t%s \nINFO:\n\t%s", e.stm_text, e.msg, e.sqlstate,e.code, e.var_info);
//#else
//        printf("数据库执行失败, \nSQL:\n\t%s \nMSG:\n\t%s \nSTATE:\n\t%s \nCODE:\n\t%s \nINFO:\n\t%s", e.stm_text, e.msg, e.sqlstate,e.code, e.var_info);
//#endif
//    }
//    catch (const std::exception& e)
//    {
//#ifdef ENABLE_SILLY_LOG
//        LOG_F(ERROR, "SILLY_OTL异常:\n\t%s", e.what());
//#else
//        printf("SILLY_OTL异常:\n\t%s", e.what());
//#endif
//    }
//    catch (...)
//    {
//        // 捕获所有其他类型的异常
//        err = "未知异常";
//    }
//    return decltype(std::invoke(std::forward<Func>(func), std::forward<Obj>(obj), std::forward<Args>(args)...))();  // 返回默认值
//}
//
//
//template <typename Func, typename Obj, typename... Args>
//auto otl_exception_wrapper(Func&& func, std::string&& err, Args&&... args) -> decltype(std::invoke(std::forward<Func>(func), std::forward<Args>(args)...))
//{
//    try
//    {
//        auto result = std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
//    }
//    catch (otl_exception& e)
//    {
//#ifdef ENABLE_SILLY_LOG
//        LOG_F(ERROR, "OTL异常, \nSQL:\n\t%s \nMSG:\n\t%s \nSTATE:\n\t%s \nCODE:\n\t%s \nINFO:\n\t%s", e.stm_text, e.msg, e.sqlstate, e.code, e.var_info);
//#else
//        printf("数据库执行失败, \nSQL:\n\t%s \nMSG:\n\t%s \nSTATE:\n\t%s \nCODE:\n\t%s \nINFO:\n\t%s", e.stm_text, e.msg, e.sqlstate, e.code, e.var_info);
//#endif
//    }
//    catch (const std::exception& e)
//    {
//#ifdef ENABLE_SILLY_LOG
//        LOG_F(ERROR, "SILLY_OTL异常:\n\t%s", e.what());
//#else
//        printf("SILLY_OTL异常:\n\t%s", e.what());
//#endif
//    }
//    catch (...)
//    {
//        // 捕获所有其他类型的异常
//        err = "未知异常";
//    }
//    return decltype(std::invoke(std::forward<Func>(func), std::forward<Args>(args)...))();  // 返回默认值
//}
//
//class i_otl_example
//{
//  public:
//    void logon()
//    {
//
//    }
//
//    void select()
//    {
//        otl_connect db;
//        const char* s_conn = "数据库连接串";
//        otl_stream query_stream;
//        std::string query_sql;
//        // db.rlogon(s_conn);
//        std::string err;
//        otl_exception_wrapper(&otl_logon, err, db, s_conn, true);
//    }
//
//    void insert()
//    {
//
//    }
//
//    void exec()
//    {
//
//    }
//
//    void idelete()
//    {
//
//    }
//};

#endif  // SILLY_UTILS_SILLY_OTL_H
