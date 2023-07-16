#pragma once
#include "json/json.h"
#include <fstream>
#include <map>

const static char* CFG_SQL_MYSQL = "MYSQL";
const static char* CFG_SQL_MSSQL = "MSSQL";
const static char* CFG_SQL_ORACLE = "ORACLE";

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
			if (sqlJson[key].isMember(CFG_SQL_MYSQL))
			{
				std::string sql = sqlJson[key][CFG_SQL_MYSQL].asString();
				m_pMYSQL.insert({ key , sql });
			}

			if (sqlJson[key].isMember(CFG_SQL_MSSQL))
			{
				std::string sql = sqlJson[key][CFG_SQL_MSSQL].asString();
				m_pMSSQL.insert({ key , sql });
			}

			if (sqlJson[key].isMember(CFG_SQL_ORACLE))
			{
				std::string sql = sqlJson[key][CFG_SQL_ORACLE].asString();
				m_pORACLE.insert({ key , sql });
			}
		}
		if (m_pMYSQL.empty() && m_pMSSQL.empty() && m_pORACLE.empty()) // 全部为空
		{
			return false;
		}
		return true;

	}

	/// <summary>
	/// 获取指定类型和功能的SQL
	/// </summary>
	/// <param name="type">数据库 类型 参照  otl_header EDBType</param>
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

};