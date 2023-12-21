//
// Created by dell on 2023/12/21.
//

#include "silly_sql_studio.h"
#include <fstream>

bool silly_sql_studio::init(const std::string cfg_path) {
    std::ifstream  ifs;
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

bool silly_sql_studio::init(const Json::Value &jv_root) {
    Json::Value::Members mem = jv_root.getMemberNames();
    for (auto iter = mem.begin(); iter != mem.end(); iter++)
    {
        std::string key = (*iter);
        if (jv_root[key].isMember(SILLY_SQL_STUDIO_MYSQL))
        {
            std::string sql = jv_root[key][SILLY_SQL_STUDIO_MYSQL].asString();
            m_pMYSQL.insert({ key , sql });
        }

        if (jv_root[key].isMember(SILLY_SQL_STUDIO_MSSQL))
        {
            std::string sql = jv_root[key][SILLY_SQL_STUDIO_MSSQL].asString();
            m_pMSSQL.insert({ key , sql });
        }

        if (jv_root[key].isMember(SILLY_SQL_STUDIO_ORACLE))
        {
            std::string sql = jv_root[key][SILLY_SQL_STUDIO_ORACLE].asString();
            m_pORACLE.insert({ key , sql });
        }
        if (jv_root[key].isMember(SILLY_SQL_STUDIO_DM8))
        {
            std::string sql = jv_root[key][SILLY_SQL_STUDIO_DM8].asString();
            m_pDM8.insert({ key , sql });
        }
        if (jv_root[key].isMember(SILLY_SQL_STUDIO_POSTGRESQL))
        {
            std::string sql = jv_root[key][SILLY_SQL_STUDIO_POSTGRESQL].asString();
            m_pDM8.insert({ key , sql });
        }
        if (jv_root[key].isMember(SILLY_SQL_STUDIO_KING_B))
        {
            std::string sql = jv_root[key][SILLY_SQL_STUDIO_KING_B].asString();
            m_pKB8.insert({ key , sql });
        }
    }
    if (m_pMYSQL.empty() && m_pMSSQL.empty() && m_pORACLE.empty() && m_pDM8.empty() && m_pPG.empty() && m_pKB8.empty()) // 全部为空
    {
        return false;
    }
    return true;
}

std::string silly_sql_studio::get(const int &type, const std::string &key) {
    std::string s_result;
    switch (type)
    {
        case enum_database_type::dbSQLSERVER: // sql server
            s_result = m_pMSSQL[key];
        case enum_database_type::dbMYSQL: // mysql
            s_result = m_pMYSQL[key];
        case enum_database_type::dbORACLE:
            s_result = m_pORACLE[key];
        case enum_database_type::dbDM8:
            s_result = m_pDM8[key];
        case enum_database_type::dbPG:
            s_result = m_pPG[key];
        case enum_database_type::dbKingB8:
            s_result = m_pKB8[key];
        default:
            s_result = m_pOTHER[key];
    }
    return s_result;
}
