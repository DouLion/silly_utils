/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: SQL.cpp
 * @description: SQL实现
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#include "SQL.h"
#define TABLE_EXIST "TABLE_EXIST"


static const std::map<std::string, std::string> MySQL2SQL = {
    {TABLE_EXIST, "SELECT * FROM information_schema.TABLES WHERE TABLE_SCHEMA = '{0}' AND TABLE_NAME = '{1}'"}
};

static const std::map<std::string, std::string> DM2SQL = {
    {TABLE_EXIST, "SELECT * FROM information_schema.TABLES WHERE TABLE_SCHEMA = '{0}' AND TABLE_NAME = '{1}'"}
};

static const std::map<std::string, std::string> Oracle2SQL = {
    {TABLE_EXIST, "SELECT * FROM information_schema.TABLES WHERE TABLE_SCHEMA = '{0}' AND TABLE_NAME = '{1}'"}
};

static const std::map<std::string, std::string> MSSQL2SQL = {
    {TABLE_EXIST, "SELECT * FROM information_schema.TABLES WHERE TABLE_SCHEMA = '{0}' AND TABLE_NAME = '{1}'"}
};

