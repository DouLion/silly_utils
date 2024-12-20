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
#define OTL_ODBC_MYSQL
#define OTL_ODBC_LOGOFF_FREES_HANDLES
#ifndef IS_WIN32
#define OTL_ODBC_UNIX

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

#define SILLY_OTL_OPT_S_IP "ip"
#define SILLY_OTL_OPT_S_PORT "port"
#define SILLY_OTL_OPT_S_TYPE "type"
#define SILLY_OTL_OPT_S_DRIVER "driver"
#define SILLY_OTL_OPT_S_SCHEMA "schema"
#define SILLY_OTL_OPT_S_USER "user"
#define SILLY_OTL_OPT_S_PASSWORD "password"
#define SILLY_OTL_OPT_S_DSN "dsn"
#define SILLY_OTL_OPT_S_VERBOSE "verbose"

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

static std::string olt_lob_stream_to_str(otl_lob_stream lob)
{
    std::string ret;
    while (!lob.eof())
    {
        std::string tmp;
        otl_long_string _sols;
        lob >> _sols;
        tmp.resize(_sols.len());
        memcpy(&tmp[0], _sols.v, _sols.len());
        ret += tmp;
    }

    return ret;
}

static std::string otl_long_str_to_str(otl_long_string ls)
{
    std::string ret;
    ret.resize(ls.len());
    memcpy(&ret[0], ls.v, ls.len());
    return ret;
}

class otl_tools;  // 始终直接传入odbc字符串时不需要这个类
class otl_conn_opt
{
    friend class otl_tools;

  public:
    /// <summary>
    /// 从字符串加载otl连接属性, 可以传入json字符串, 也可以直接传入odbc字符串
    /// </summary>
    /// <param name="cfg">ODBC 或者 JSON字符串</param>
    /// <returns></returns>
    bool load(const std::string& cfg);

    /// <summary>
    /// 获取odbc连接串
    /// </summary>
    /// <param name="rebuild"></param>
    /// <returns></returns>
    std::string odbc(const bool& rebuild = false);

    /// <summary>
    /// 列举本机的所有ODBC驱动
    /// </summary>
    /// <returns></returns>
    static std::vector<std::string> drivers();

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
    static void help();

    /// <summary>
    /// 获取数据库编码 TODO: 实现有问题
    /// </summary>
    /// <returns></returns>
    std::string encode();

    /// <summary>
    /// 非session执行sql的模板函数
    /// </summary>
    /// <param name="Func"></param>
    /// <param name="...Args"></param>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="...args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool execute(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(INT_MAX - 1);
            db.rlogon(m_conn.c_str(), true);
            if(m_verbose)
            {
                SLOG_INFO("SQL:{}", sql);
            }

            /* otl_stream stream;
             stream.open(1, sql.c_str(), db);*/
            func(&db, std::forward<Args>(args)...);
            db.commit();
            // stream.close();

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

    /// <summary>
    /// 直接执行不需要参数的sql
    /// </summary>
    /// <param name="sql"></param>
    /// <returns></returns>
    bool execute(const std::vector<std::string>& sqls)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.rlogon(m_conn.c_str(), false);
            db.auto_commit_off();

            for (auto sql : sqls)
            {
                if(m_verbose)
                {
                    SLOG_INFO("SQL:{}", sql);
                }
                db.direct_exec(sql.c_str());
            }

            db.commit();
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

    template <typename Func, typename... Args>
    bool session(Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(INT_MAX - 1);
            db.rlogon(m_conn.c_str(), false);
            db.auto_commit_off();


            func(&db, std::forward<Args>(args)...);
            db.commit();
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

    /// <summary>
    /// select的模板函数
    /// </summary>
    /// <param name="Func"></param>
    /// <param name="...Args"></param>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="...args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool select(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(INT_MAX - 1);
            db.rlogon(m_conn.c_str());
            if(m_verbose)
            {
                SLOG_INFO("SQL:{}", sql);
            }
            otl_stream stream;
            stream.set_lob_stream_mode(true);
            stream.open(1, sql.c_str(), db);
            func(&stream, std::forward<Args>(args)...);
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

    /// <summary>
    /// insert的模板函数
    /// </summary>
    /// <param name="Func"></param>
    /// <param name="...Args"></param>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="...args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool insert(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.auto_commit_off();
            db.set_timeout(m_timeout);
            db.rlogon(m_conn.c_str(), false);
            if(m_verbose)
            {
                SLOG_INFO("SQL:{}", sql);
            }
            db.set_max_long_size(50*SU_MB);
            otl_stream stream;
            stream.open(1, sql.c_str(), db);
            func(&stream, std::forward<Args>(args)...);
            stream.flush();
            stream.close();
            db.commit();
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

    /// <summary>
    /// 使用lob(Large Object Binary)插入
    /// </summary>
    /// <param name="Func"></param>
    /// <param name="...Args"></param>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="...args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool insert_lob(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.auto_commit_off();
            db.set_timeout(m_timeout);
            db.rlogon(m_conn.c_str(), false);
            if(m_verbose)
            {
                SLOG_INFO("SQL:{}", sql);
            }

            otl_stream stream;
            stream.set_lob_stream_mode(true);
            stream.open(1, sql.c_str(), db);
            func(&stream, std::forward<Args>(args)...);
            stream.flush();
            stream.close();
            db.commit();
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

    /// <summary>
    /// 查询sql的列信息
    /// </summary>
    /// <param name="sql"></param>
    /// <returns>执行是否成功</returns>
    bool check_column_info(const std::string& sql);

    /// <summary>
    /// otl类型枚举对应的字符串
    /// </summary>
    /// <param name="sql"></param>
    /// <returns>执行是否成功</returns>
    static std::string otl_type_name(const otl_var_enum& vt);

    ///////////////////////////////
    /// getter
    ///////////////////////////////
    enum_database_type type() const;
    std::string driver() const;
    std::string ip() const;
    int port() const;
    std::string schema() const;
    std::string user() const;
    std::string pwd() const;
    std::string err() const;

    ///////////////////////////////
    /// setter
    ///////////////////////////////
    void type(enum_database_type tp);
    void driver(std::string d);
    void ip(std::string i);
    void port(int p);
    void schema(std::string s);
    void user(std::string u);
    void pwd(std::string p);
    void timeout(int to);
    void verbose(bool vb);

  protected:
    std::string m_ip;
    int m_port{0};
    enum_database_type m_type{enum_database_type::dbINVALID};
    std::string m_driver;  // 驱动名称 可由
    std::string m_schema;
    std::string m_user;
    std::string m_password;
    std::string m_dsn;
    int m_timeout{10}; // 单位 秒
    std::string m_conn;
    std::string m_err;
    bool m_verbose = false;
};

using silly_otl = otl_conn_opt;

/* ODBC 示例
  Driver={DM8 ODBC DRIVER};Server=127.0.0.1;TCP_PORT=5236;UID=SYSDBA;PWD=xxxxxxxx;
  Driver={Oracle in OraClient12home1};DBQ=192.168.0.71/ORCL;Uid=RWDB_HN;Pwd=xxxxxxx;
  Driver={MySQL ODBC 8.0 ANSI Driver};Server=192.168.0.154;Port=3306;Database=RWDB_XJ;User=root;Password=xxxx;Option=3;
  Driver={sql server};server=192.168.0.151;port=1433;uid=sa;pwd=3edc9ijn~;database=RWDB_HB_ALL
  Driver={PostgreSQL};Server=IP address;Port=5432;Database=myDataBase;Uid=myUsername;Pwd=myPassword;
*/
/* JSON示例
    {
        "type": "dm8",
        "ip": "192.168.0.179",
        "port": 5238,
        "schema": "TZX_DYNAMIC_ANALYSIS_HUBEI",
        "user": "SYSDBA",
        "password": "xxxx",
        "driver": "DM8 ODBC Driver"
    }
*/
/* DSN 示例
    UID=sa;PWD=xxx;DSN=RWDB_DSN;
*/

#endif  // SILLY_UTILS_SILLY_OTL_H
