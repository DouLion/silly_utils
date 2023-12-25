/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/5/17 10:52
 * @version: 1.0.1
 * @software: Project
 * @description:
 */
#pragma once

#ifndef OTL_TOOLS_H
#define OTL_TOOLS_H

#include "database/otl/silly_otl.h"
#include <json/json.h>

class otl_tools
{
public:

	static otl_datetime otl_time_from_string(const std::string &str);

	static std::string otl_time_to_string(const otl_datetime &tm);

	static otl_conn_opt conn_opt_from_json(const Json::Value &root);

	static otl_conn_opt conn_opt_from_json(const std::string &json_str);

	static std::vector<std::string> get_local_odbc_drivers();

};

#endif // OTL_TOOLS_H
