//
// Created by dell on 2023/12/21.
//

#include "silly_sql_studio.h"
#include <fstream>

bool silly_sql_studio::init(const std::string cfg_path)
{
    std::ifstream ifs;
    ifs.open(cfg_path, std::ios::in);
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(ifs, root))
    {
        return init(root);
    }

    return false;
    return false;
}

bool silly_sql_studio::init(const Json::Value &jv_root)
{
    Json::Value::Members mem = jv_root.getMemberNames();
    for (auto iter = mem.begin(); iter != mem.end(); iter++)
    {
        std::string key = (*iter);
        if (jv_root[key].isMember(SILLY_DB_TYPE_MYSQL_STR))
        {
            std::string sql = jv_root[key][SILLY_DB_TYPE_MYSQL_STR].asString();
            m_pMYSQL.insert({key, sql});
        }

        if (jv_root[key].isMember(SILLY_DB_TYPE_MSSQL_STR))
        {
            std::string sql = jv_root[key][SILLY_DB_TYPE_MSSQL_STR].asString();
            m_pMSSQL.insert({key, sql});
        }

        if (jv_root[key].isMember(SILLY_DB_TYPE_ORACLE_STR))
        {
            std::string sql = jv_root[key][SILLY_DB_TYPE_ORACLE_STR].asString();
            m_pORACLE.insert({key, sql});
        }
        if (jv_root[key].isMember(SILLY_DB_TYPE_DM8_STR))
        {
            std::string sql = jv_root[key][SILLY_DB_TYPE_DM8_STR].asString();
            m_pDM8.insert({key, sql});
        }
        if (jv_root[key].isMember(SILLY_DB_TYPE_POSTGRESQL_STR))
        {
            std::string sql = jv_root[key][SILLY_DB_TYPE_POSTGRESQL_STR].asString();
            m_pDM8.insert({key, sql});
        }
        if (jv_root[key].isMember(SILLY_DB_TYPE_KINGB8_STR))
        {
            std::string sql = jv_root[key][SILLY_DB_TYPE_KINGB8_STR].asString();
            m_pKB8.insert({key, sql});
        }
    }
    if (m_pMYSQL.empty() && m_pMSSQL.empty() && m_pORACLE.empty() && m_pDM8.empty() && m_pPG.empty() && m_pKB8.empty())  // 全部为空
    {
        return false;
    }
    return true;
}

std::string silly_sql_studio::get(const int &type, const std::string &key)
{
    std::string s_result;
    enum_database_type ntype = static_cast<enum_database_type>(type);
    switch (ntype)
    {
        case enum_database_type::dbSQLSERVER:  // sql server
            s_result = m_pMSSQL[key];
            break;
        case enum_database_type::dbMYSQL:  // mysql
            s_result = m_pMYSQL[key];
            break;
        case enum_database_type::dbORACLE:
            s_result = m_pORACLE[key];
            break;
        case enum_database_type::dbDM8:
            s_result = m_pDM8[key];
            break;
        case enum_database_type::dbPG:
            s_result = m_pPG[key];
            break;
        case enum_database_type::dbKingB8:
            s_result = m_pKB8[key];
            break;
        default:
            s_result = m_pOTHER[key];
            break;
    }
    return s_result;
}
