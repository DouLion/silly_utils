#pragma once
#include "json/json.h"
#include <fstream>
#include <map>

#define SU_SQL_STUDIO_MYSQL			"MYSQL"
#define SU_SQL_STUDIO_MSSQL			"MSSQL"
#define SU_SQL_STUDIO_ORACLE		"ORACLE"
#define SU_SQL_STUDIO_DM8			"DM8"
#define SU_SQL_STUDIO_POSTGRE		"POSTGRESQL"
#define SU_SQL_STUDIO_OTHER			"OTHER"

class SqlStudio
{
public:
	bool Initialize(const std::string& sqlCfgPath)
	{
		std::ifstream  sqlCfgFile;
		sqlCfgFile.open(sqlCfgPath, std::ios::in);
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(sqlCfgFile, root))
		{
			return Initialize(root);
		}
		
		return false;

	}
	bool Initialize(const Json::Value& sqlJson)
	{
		Json::Value::Members mem = sqlJson.getMemberNames();
		for (auto iter = mem.begin(); iter != mem.end(); iter++)
		{
			std::string key = (*iter);
			if (sqlJson[key].isMember(SU_SQL_STUDIO_MYSQL))
			{
				std::string sql = sqlJson[key][SU_SQL_STUDIO_MYSQL].asString();
				m_pMYSQL.insert({ key , sql });
			}

			if (sqlJson[key].isMember(SU_SQL_STUDIO_MSSQL))
			{
				std::string sql = sqlJson[key][SU_SQL_STUDIO_MSSQL].asString();
				m_pMSSQL.insert({ key , sql });
			}

			if (sqlJson[key].isMember(SU_SQL_STUDIO_ORACLE))
			{
				std::string sql = sqlJson[key][SU_SQL_STUDIO_ORACLE].asString();
				m_pORACLE.insert({ key , sql });
			}
			if (sqlJson[key].isMember(SU_SQL_STUDIO_DM8))
			{
				std::string sql = sqlJson[key][SU_SQL_STUDIO_DM8].asString();
				m_pDM8.insert({ key , sql });
			}
			if (sqlJson[key].isMember(SU_SQL_STUDIO_POSTGRE))
			{
				std::string sql = sqlJson[key][SU_SQL_STUDIO_POSTGRE].asString();
				m_pDM8.insert({ key , sql });
			}
			if (sqlJson[key].isMember(SU_SQL_STUDIO_OTHER))
			{
				std::string sql = sqlJson[key][SU_SQL_STUDIO_OTHER].asString();
				m_pOTHER.insert({ key , sql });
			}
		}
		if (m_pMYSQL.empty() && m_pMSSQL.empty() && m_pORACLE.empty() && m_pDM8.empty() && m_pPG.empty() && m_pOTHER.empty()) // 全部为空
		{
			return false;
		}
		return true;

	}

	/// <summary>
	/// 获取指定类型和功能的SQL
	/// </summary>
	/// <param name="type">数据库 类型 参照  otl_header.hpp EDBType</param>
	/// <param name="key"></param>
	/// <returns></returns>
	std::string Get(const int& type, const std::string& key)
	{
		switch (type)
		{
		case 1: // sql server
			return m_pMSSQL[key];
		case 2: // mysql
			return m_pMYSQL[key];
		case 3:
			return m_pORACLE[key];
		case 4:
			return m_pDM8[key];
		case 5:
			return m_pPG[key];
		default:
			return "";
		}
		// unreach
		return "";
	}


private:

	std::map<std::string, std::string> m_pMYSQL;
	std::map<std::string, std::string> m_pMSSQL;
	std::map<std::string, std::string> m_pORACLE;
	std::map<std::string, std::string> m_pDM8;
	std::map<std::string, std::string> m_pPG;
	std::map<std::string, std::string> m_pOTHER;

};