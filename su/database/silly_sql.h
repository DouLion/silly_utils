/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-26
 * @file: silly_sql.h
 * @description: 一些常用的SQL
 * @version: v1.0.1 2024-12-26 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SQL_H
#define SILLY_UTILS_SILLY_SQL_H
#include <iostream>
namespace silly
{
namespace sql
{
namespace dm8
{
// 建表语句 模式名  表名  区分大小写
static const std::string DLL = "CALL SP_TABLEDEF('{}', '{}')";

static const std::string LAST_INSERT_ID = "SELECT SCOPE_IDENTITY();";
}  // namespace dm8

namespace oracle
{
//
static const std::string DLL = "";

static const std::string LAST_INSERT_ID = "SELECT @@IDENTITY";
}  // namespace oracle

namespace mysql
{
// 库名 表名 区分大小写
static const std::string DLL = "SHOW CREATE TABLE {}.{} ;";
static const std::string LAST_INSERT_ID = "SELECT @@IDENTITY";
}  // namespace mysql

namespace sqlserver
{
// 库名 表名 区分大小写
static const std::string DLL = "SHOW CREATE TABLE {}.{} ;";
static const std::string LAST_INSERT_ID = "SELECT @@IDENTITY";
}  // namespace sqlserver

namespace postgresql
{
//
static const std::string DLL = "";
static const std::string LAST_INSERT_ID = "SELECT @@IDENTITY";
}  // namespace postgresql

namespace sqlite
{
// 表名
static const std::string DLL = ".schema {}";
static const std::string LAST_INSERT_ID = "SELECT @@IDENTITY";
}  // namespace sqlite

namespace kingb8
{
//
static const std::string DLL = "";
static const std::string LAST_INSERT_ID = "SELECT @@IDENTITY";
}  // namespace kingb8


}  // namespace sql
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_SQL_H
