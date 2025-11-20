/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/21 13:42
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_OTL_H
#define SILLY_OTL_H
#include <su_marco.h>
#define OTL_ODBC
#define OTL_ODBC_LOGOFF_FREES_HANDLES
#ifndef _WIN32
#define OTL_ODBC_UNIX
#else

#endif

#define OTL_STREAM_READ_ITERATOR_ON  // image 等大量数据插入的支持
#define OTL_STL                      // 支持用std::string 接受char* 类型

#define OTL_ANSI_CPP                        // 中文支持
#define OTL_ANSI_CPP_11_VARIADIC_TEMPLATES  // 支持 otl_value< >
#define OTL_CPP_17_ON                       // 支持连接池的 c++ 版本
#define OTL_CONNECT_POOL_ON                 // 连接池支持

#ifndef NOMINMAX  // std::max std::min  c++ 17 版本的一些问题
#define NOMINMAX
#endif

#ifndef _CRT_SECURE_NO_WARNINGS  // 函数安全
#define _CRT_SECURE_NO_WARNINGS
#endif

#if SU_THIRD_SUPPORT_JSONPP
#include <json/silly_jsonpp.h>
#endif

#include <database/otl/otlv4.h>
#include <database/silly_sql.h>
#include <system/silly_system.h>

#include <log/silly_log.h>

#ifdef IS_WIN32
#pragma comment(lib, "odbc32.lib")
#endif

#define SILLY_OTL_ODBC_MAX_LEN 1024

enum class eOtlDbType
{
    dbINVALID = 0,    // 无效数据源类型
    dbSQLSERVER = 1,  // SQLSERVER 数据库
    dbMYSQL = 2,      // MYSQL 数据库
    dbORACLE = 3,     // ORACLE 数据库
    dbDM8 = 4,        // 达梦
    dbPG = 5,         // postgres
    dbKingB8 = 6,     // 人大金仓
    dbMariaDB = 7     // MYSQL的一个开源分支,基本能够兼容mysql
};


class suOTL
{
  public:
    static std::string LStr2Str(const otl_long_string& lstr);
    static std::string Lob2Str(otl_lob_stream* stream);
    static std::string Time2Str(const otl_datetime& dt, const bool& millisecond = false);
    static otl_datetime Str2Time(const std::string& str);

    /// @brief 时间戳转换为+8区时间
    /// @param[in] dt
    /// @return 时间戳
    static std::time_t Time2Stamp(const otl_datetime& dt);

    /// @brief +8区时间转换为时间戳
    /// @param[in] stamp
    /// @return 时间
    static otl_datetime Stamp2Time(const std::time_t& stamp);

    static std::string VarType2Name(const otl_var_enum& vt);

    static eOtlDbType Str2DbType(const std::string& desc);

    static std::string DbType2Str(const eOtlDbType& type);

    /// <summary>
    /// 从字符串加载otl连接属性, 可以传入json字符串, 也可以直接传入odbc字符串
    /// </summary>
    /// <param name="cfg">ODBC 或者 JSON字符串</param>
    /// <returns></returns>
    bool load(const std::string& cfg);

    suOTL& operator=(const suOTL& rh);

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
    bool check(const bool& rebuild_odbc = true);

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

    std::string last_insert_id_sql() const;

    /// <summary>
    /// 非session执行sql的模板函数
    /// </summary>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool execute(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(50 * SU_MB);
            db.rlogon(m_conn.c_str(), true);
            if (m_verbose)
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

    /// <summary>
    /// 直接执行不需要参数的sql
    /// </summary>
    /// <param name="sqls"></param>
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
                if (m_verbose)
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

    template <typename Func, typename... Args>
    bool session(Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(50 * SU_MB);
            db.rlogon(m_conn.c_str(), false);
            db.auto_commit_off();

            func(&db, std::forward<Args>(args)...);
            db.commit();
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

    /// <summary>
    /// select的模板函数
    /// </summary>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool select(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(50 * SU_MB);
            db.rlogon(m_conn.c_str());
            if (m_verbose)
            {
                SLOG_INFO("SQL:{}", sql);
            }
            otl_stream stream;
            stream.open(1, sql.c_str(), db);
            func(&stream, std::forward<Args>(args)...);
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

    /// <summary>
    /// 存储过程查询的模板函数, 获取结果同select
    /// </summary>
    /// <param name="sql">{call my_proc(:f1<int,in>)}</param>
    /// <param name="func"></param>
    /// <param name="args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool procedure(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(50 * SU_MB);
            db.rlogon(m_conn.c_str());
            if (m_verbose)
            {
                SLOG_INFO("SQL:{}", sql);
            }
            otl_stream stream;
            stream.open(1, sql.c_str(), db, otl_implicit_select);
            func(&stream, std::forward<Args>(args)...);
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

    /// <summary>
    /// select的模板函数, 以lob的方式读取large object binary
    /// </summary>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="args"></param>
    /// <returns>执行是否成功</returns>
    template <typename Func, typename... Args>
    bool select_lob(const std::string& sql, Func&& func, Args&&... args)
    {
        bool status = false;
        otl_connect db;
        try
        {
            db.set_timeout(m_timeout);
            db.set_max_long_size(50 * SU_MB);
            db.rlogon(m_conn.c_str());
            if (m_verbose)
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

    /// <summary>
    /// insert的模板函数
    /// </summary>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="args"></param>
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
            if (m_verbose)
            {
                SLOG_INFO("SQL:{}", sql);
            }
            db.set_max_long_size(50 * SU_MB);
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

    /// <summary>
    /// 使用lob(Large Object Binary)插入
    /// </summary>
    /// <param name="sql"></param>
    /// <param name="func"></param>
    /// <param name="args"></param>
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
            if (m_verbose)
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

    /// <summary>
    /// 查询sql的列信息
    /// </summary>
    /// <param name="sql"></param>
    /// <returns>执行是否成功</returns>
    bool check_column_info(const std::string& sql);

    void make_otl_err(otl_exception& e)
    {
        m_err = "OTL_ERR \nCONN:";
        m_err.append(m_conn);
        m_err.append("\nCODE:").append(std::to_string(e.code));
        m_err.append("\nMSG:").append(std::string((char*)e.msg));
        m_err.append("\nSTATE:").append(std::string((char*)e.sqlstate));
        m_err.append("\nSTMT:").append(std::string((char*)e.stm_text));
    }
    ///////////////////////////////
    /// getter
    ///////////////////////////////
    eOtlDbType type() const;
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
    void type(const eOtlDbType& tp);
    void driver(const std::string& d);
    void ip(const std::string& i);
    void port(const int& p);
    void schema(const std::string& s);
    void user(const std::string& u);
    void pwd(const std::string& p);
    void timeout(const int& to);
    void verbose(const bool& vb);

#if SU_THIRD_SUPPORT_JSONPP
    bool from_json(const std::string& jstr);
    bool from_json(const Json::Value& jv);

#endif

  protected:
    std::string m_ip;
    int m_port{0};
    eOtlDbType m_type{eOtlDbType::dbINVALID};
    std::string m_driver;  // 驱动名称 可由
    std::string m_schema;
    std::string m_user;
    std::string m_password;
    std::string m_dsn;
    int m_timeout{10};  // 单位 秒
    std::string m_conn;
    std::string m_err;
    bool m_verbose = false;
};

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

#endif  // SILLY_OTL_H
