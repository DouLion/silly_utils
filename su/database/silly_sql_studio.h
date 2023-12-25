/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/21 13:41
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_SQL_STUDIO_H
#define SILLY_UTILS_SILLY_SQL_STUDIO_H

#include <map>
#include <iostream>
#include <json/json.h>
#include <database/otl/silly_otl.h>

class silly_sql_studio
{
    bool init(const std::string cfg_path);
    bool init(const Json::Value& jv_root);
    std::string get(const int& type, const std::string& key);

private:

    std::map<std::string, std::string> m_pMYSQL;
    std::map<std::string, std::string> m_pMSSQL;
    std::map<std::string, std::string> m_pORACLE;
    std::map<std::string, std::string> m_pDM8;
    std::map<std::string, std::string> m_pPG;
    std::map<std::string, std::string> m_pKB8;
    std::map<std::string, std::string> m_pOTHER;
};

#endif //SILLY_UTILS_SILLY_SQL_STUDIO_H
