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
#include <json/silly_jsonpp.h>

class otl_tools
{
  public:
    static otl_datetime otl_time_from_string(const std::string &str);

    static std::string otl_time_to_string(const otl_datetime &tm);

    static silly_otl conn_opt_from_json(const Json::Value &root);
    static silly_otl conn_opt_from_json(const std::string &jstr);

    static bool  conn_opt_from_json(const std::string &str, silly_otl& ret);
    static bool  conn_opt_from_json(const Json::Value &root, silly_otl& ret);
};

#endif  // OTL_TOOLS_H
