/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_sqlite3.cpp
 * @description: silly_sqlite3实现
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#include "silly_sqlite3.h"
#include <files/silly_file.h>

const char* SQL_BEGIN_WAL_MODE[] = {"PRAGMA journal_mode=WAL;", "PRAGMA synchronous = NORMAL;", "PRAGMA wal_autocheckpoint = 1000;", "PRAGMA busy_timeout = 5000;"};
const char* SQL_JOURNAL_NODE = "PRAGMA journal_mode=OFF;";
bool suSQLite3::Memory()
{
    if (m_db)
    {
        m_err = "数据库已经打开";
        throw std::runtime_error("已经打开的数据库: [" + m_file.string() + "]");
    }
    if (SQLITE_OK == sqlite3_open(":memory:", &m_db))
    {
        m_mem = true;
        return true;
    }
    return false;
}
bool suSQLite3::Open(const suPath& file)
{
    if (m_db)
    {
        m_err = "数据库已经打开";
        throw std::runtime_error("已经打开的数据库: [" + m_file.string() + "]");
    }
    if (SQLITE_OK != sqlite3_open(file.string().c_str(), &m_db) || !m_db)
    {
        return false;
    }
    char* errmsg = nullptr;
    for (const char* sql : SQL_BEGIN_WAL_MODE)
    {
        if (SQLITE_OK != sqlite3_exec(m_db, sql, nullptr, nullptr, &errmsg))
        {
            std::cerr << "SQLite3 exec: " << sql << std::endl;
            std::cerr << "SQLite3: " << errmsg << std::endl;
            return false;
        }
    }

    return true;
}

void suSQLite3::Close()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool suSQLite3::VACUUM() const
{
    return sqlite3_exec(m_db, "VACUUM;", nullptr, nullptr, nullptr) == SQLITE_OK;
}
sqlite3* suSQLite3::DB() const
{
    return m_db;
}
std::string suSQLite3::Err() const
{
    return m_err;
}
bool suSQLite3::Backup(const suPath& file) const
{
    /*if (m_file == file)
    {
        m_err = "同一个数据库文件无法备份";
        return false;
    }*/

    // 将内存数据库备份到文件
    sqlite3* backup_db;
    int rc = sqlite3_open(file.string().c_str(), &backup_db);
    if (rc)
    {
        std::cerr << "Can't open disk database: " << sqlite3_errmsg(backup_db) << std::endl;
        return false;
    }

    sqlite3_backup* pBackup = sqlite3_backup_init(backup_db, "main", m_db, "main");
    if (pBackup == NULL)
    {
        std::cerr << "Backup initialization failed: " << sqlite3_errmsg(backup_db) << std::endl;
        sqlite3_close(backup_db);
        return false;
    }

    while ((rc = sqlite3_backup_step(pBackup, 100)) == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED)
    {
        // Wait or retry as necessary
        sqlite3_close(backup_db);
        return false;
    }

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Backup step failed: " << sqlite3_errmsg(backup_db) << std::endl;
        return false;
    }

    sqlite3_backup_finish(pBackup);
    sqlite3_close(backup_db);
    return true;
}

bool suSQLite3::CreateTable(const std::string& tbnm, const suSQLite3::Table& tbl, const bool& cover)
{
    if (!m_db)
    {
        return false;
    }

    std::string sql = tbl.GenDLLSQL();
    if (SQLITE_OK != sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, nullptr))
    {
        m_err = "创建表失败: " + sql;
        return false;
    }
    return true;
}

suSQLite3::Table suSQLite3::GetTable(const std::string& tbnm) const
{
    if (m_table2dll.find(tbnm) != m_table2dll.end())
    {
        return m_table2dll.at(tbnm);
    }
    return {};
}

suSQLite3::Table& suSQLite3::Table::AddCol(const std::string& colnm, const eSQLite3Type& type, const bool& primary)
{
    cols.push_back({colnm, type, primary});
    return *this;
}
suSQLite3::Table& suSQLite3::Table::AddCol(const std::string& colnm, const std::string& type, const bool& primary)
{
    eSQLite3Type EnumType = eSQLite3Type::INVALID;
    std::string UpType = type;
    std::transform(UpType.begin(), UpType.end(), UpType.begin(), ::toupper);

    if (UpType == "AUTOINC")
    {
        EnumType = eSQLite3Type::AUTOINC;
    }
    else if (UpType == "INTEGER")
    {
        EnumType = eSQLite3Type::INTEGER;
    }
    else if (UpType == "TEXT")
    {
        EnumType = eSQLite3Type::TEXT;
    }
    else if (UpType == "REAL")
    {
        EnumType = eSQLite3Type::REAL;
    }
    else if (UpType == "BLOB")
    {
        EnumType = eSQLite3Type::BLOB;
    }
    else
    {
        return *this;
    }
    return AddCol(colnm, EnumType, primary);
}

std::string SQLite3TypeToStr(const eSQLite3Type& type)
{
    switch (type)
    {
        case eSQLite3Type::AUTOINC:
            return "INTEGER PRIMARY KEY AUTOINCREMENT";
        case eSQLite3Type::BLOB:
            return "BLOB";
        case eSQLite3Type::INTEGER:
            return "INTEGER";
        case eSQLite3Type::REAL:
            return "REAL";
        case eSQLite3Type::TEXT:
            return "TEXT";
    }
    return "";
}
std::string suSQLite3::Table::GenDLLSQL() const
{
    std::string sql = "CREATE TABLE IF NOT EXISTS " + name + "(";
    // std::string indexcols = "";
    for (auto& col : cols)
    {
        std::string cnm = std::get<0>(col);
        eSQLite3Type tp = std::get<1>(col);
        bool primary = std::get<2>(col);


        sql += cnm + " " + SQLite3TypeToStr(tp);
        if (primary && tp != eSQLite3Type::AUTOINC)
        {
            sql += " PRIMARY KEY";
        }
        sql += ",";
    }
    sql.pop_back();
    sql += ");";
    return sql;
}

std::string suSQLite3::Table::GenViewSQL() const
{
    std::string view_name = "V12W_" + name;
    std::string sql = "CREATE VIEW IF NOT EXISTS " + view_name + " AS SELECT";
    for (const auto& [col, visable] : visables)
    {
        if (visable)
        {
            sql += col + ",";
        }
    }
    sql.pop_back();
    sql += " FROM " + name + ";";
    return sql;
}

std::string suSQLite3::Table::GenDropSQL() const
{
    return "DROP TABLE IF EXISTS " + name + ";";
}
