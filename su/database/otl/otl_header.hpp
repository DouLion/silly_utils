///*
// * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
// * reserved.
// * @website: http://www.tianzhixiang.com.cn/
// * @author: dou li yang
// * @date: 2023/5/17 10:51
// * @version: 1.0.1
// * @software: Project
// * @description: OTL查用的方法和预定义的变量
// */
//#pragma once
//
//#ifndef OTL_HEADER_HPP
//#define OTL_HEADER_HPP
//
//#define OTL_ODBC
//
//#ifndef IS_WIN32
//#define OTL_ODBC_UNIX
//#define OTL_ODBC_MYSQL
//#else
//
//#endif
//
//#define OTL_STREAM_READ_ITERATOR_ON // image 等大量数据插入的支持
//#define OTL_STL						// 支持用std::string 接受char* 类型
//
//#define OTL_ANSI_CPP					   // 中文支持
//#define OTL_ANSI_CPP_11_VARIADIC_TEMPLATES // 支持 otl_value< >
//#define OTL_CPP_11_ON					   // 支持连接池的 c++ 版本
//#define OTL_CONNECT_POOL_ON				   // 连接池支持
//
//#ifndef NOMINMAX // std::max std::min  c++ 17 版本的一些问题
//#define NOMINMAX
//#endif
//
//#ifndef _CRT_SECURE_NO_WARNINGS // 函数安全
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//
//#include <database/otl/otlv4.h>
//
//#ifdef IS_WIN32
//#pragma comment(lib, "odbc32.lib")
//#endif
//
//using OtlConnPool = otl_connect_pool<otl_connect, otl_exception>; // 不建议使用连接池
//
//enum class EDBType
//{
//	dbINVALID = 0,	 // 无效数据源类型
//	dbSQLSERVER = 1, // SQLSERVER 数据库
//	dbMYSSQL = 2,	 // MYSQL 数据库
//	dbORACLE = 3,	 // ORACLE 数据库
//	dbDM8 = 4,		 // 达梦
//	dbPG = 5,		 // postgres
//};
//
//// longtext varchar(max) 等数据转字符串， 限定长度的 text 和 varchar 用 std::string 直接获取
//
//#define OTL_DB_TYPE_STR_MSSQL "sqlserver"
//#define OTL_DB_TYPE_STR_MYSQL "mysql"
//#define OTL_DB_TYPE_STR_ORACLE "oracle"
//#define OTL_DB_TYPE_STR_DM8 "dm8"
//#define OTL_DB_TYPE_STR_PG "postgresql"
//
//static EDBType GetType(const std::string &desc)
//{
//	if (OTL_DB_TYPE_STR_MSSQL == desc)
//	{
//		return EDBType::dbSQLSERVER;
//	}
//	else if (OTL_DB_TYPE_STR_MYSQL == desc)
//	{
//		return EDBType::dbMYSSQL;
//	}
//	else if (OTL_DB_TYPE_STR_ORACLE == desc)
//	{
//		return EDBType::dbORACLE;
//	}
//	else if (OTL_DB_TYPE_STR_DM8 == desc)
//	{
//		return EDBType::dbDM8;
//	}
//	else if (OTL_DB_TYPE_STR_PG == desc)
//	{
//		return EDBType::dbPG;
//	}
//	return EDBType::dbINVALID;
//}
//
//static std::string DBTypeToString(const EDBType &type)
//{
//	switch (type)
//	{
//	case EDBType::dbMYSSQL:
//		return OTL_DB_TYPE_STR_MSSQL;
//	case EDBType::dbSQLSERVER:
//		return OTL_DB_TYPE_STR_MYSQL;
//	case EDBType::dbORACLE:
//		return OTL_DB_TYPE_STR_ORACLE;
//	case EDBType::dbDM8:
//		return OTL_DB_TYPE_STR_DM8;
//	case EDBType::dbPG:
//		return OTL_DB_TYPE_STR_PG;
//	default:
//		break;
//	}
//	return "";
//}
//
//#define OTL_OPT_JSON_IP "ip"
//#define OTL_OPT_JSON_PORT "port"
//#define OTL_OPT_JSON_TYPE "type"
//#define OTL_OPT_JSON_DRIVER "driver"
//#define OTL_OPT_JSON_SCHEMA "schema"
//#define OTL_OPT_JSON_USER "user"
//#define OTL_OPT_JSON_PASSWORD "password"
//#define OTL_OPT_JSON_POOLNUM "poolnum"
//#define OTL_OPT_JSON_DSN "dsn"
//
//class OtlConnOption
//{
//public:
//	OtlConnOption()
//	{
//
//		ip = "";
//		port = -1;
//		type = EDBType::dbSQLSERVER;
//		driver = "";
//		schema = "";
//		user = "";
//		password = "";
//		dsn = "";
//		m_pOtlConnStr = "";
//		poolnum = 8;
//	}
//
//	std::string ip;
//	int port;
//	EDBType type;
//	std::string driver;
//	std::string schema;
//	std::string user;
//	std::string password;
//	std::string dsn;
//	int poolnum;
//
//	/// <summary>
//	/// 生成对应的odbc链接串
//	/// </summary>
//	/// <param name="rebuild">是否重新生产连接串, 属性修改后需要设置true 使连接串更新</param>
//	/// <returns></returns>
//	std::string GetOtlConnString(const bool &rebuild = false)
//	{
//		if (rebuild || m_pOtlConnStr.empty())
//		{
//			if (!dsn.empty()) // 如果ODBC连接串不好使,设置DSN,并且优先使用DSN链接方式
//			{
//				m_pOtlConnStr = "UID=" + user + ";PWD=" + password + ";DSN=" + dsn + ";";
//				return m_pOtlConnStr;
//			}
//			switch (type) // 使用ODBC连接串
//			{
//			case EDBType::dbMYSSQL:
//				m_pOtlConnStr =
//					"Driver={" + driver + "};Server=" + ip + ";Port=" + std::to_string(port) + ";Database=" +
//					schema + ";User=" + user + ";Password=" + password + ";Option=3;charset=UTF8;";
//				break;
//			case EDBType::dbSQLSERVER:
//				m_pOtlConnStr =
//					"Driver={" + driver + "};Server=" + ip + ";Port:" + std::to_string(port) + ";Database=" +
//					schema + ";UID=" + user + ";PWD=" + password + ";";
//				break;
//			case EDBType::dbORACLE:
//				m_pOtlConnStr = "Driver={" + driver + "};DBQ=" + ip + ":" + std::to_string(port) + "/" + schema +
//								";Uid=" + user + ";Pwd=" + password + ";";
//				break;
//			default:
//				break;
//			}
//		}
//		return m_pOtlConnStr;
//	}
//
//	OtlConnOption &operator=(const OtlConnOption &other)
//	{
//		this->type = other.type;
//		this->ip = other.ip;
//		this->port = other.port;
//		this->driver = other.driver;
//		this->schema = other.schema;
//		this->user = other.user;
//		this->password = other.password;
//		this->poolnum = other.poolnum;
//		this->dsn = other.dsn;
//
//		return *this;
//	}
//
//protected:
//	std::string m_pOtlConnStr;
//};
//
//#endif // OTL_HEADER_HPP
