/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_http_server.cpp
 * @description: silly_http_server实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_http_server.h"

std::string suHttpUtils::CheckStr(const std::unordered_map<std::string, std::string>& k2v, const std::string& key, const std::string& dv)
{
    std::string lkey = TO_LOWER(key);
    if (k2v.find(lkey) != std::end(k2v))
    {
        return k2v.at(lkey);
    }
    return dv;
}
std::string suHttpUtils::TryStr(const std::unordered_map<std::string, std::string>& k2v, const std::string& key)
{
    std::string lkey = TO_LOWER(key);
    if (k2v.find(lkey) != std::end(k2v))
    {
        return k2v.at(lkey);
    }
    throw std::invalid_argument("缺少参数: " + key);
}

double suHttpUtils::CheckNum(const std::unordered_map<std::string, std::string>& k2v, const std::string& key, const double& dv)
{
    std::string lkey = TO_LOWER(key);
    if (k2v.find(lkey) != std::end(k2v))
    {
        return std::stod(k2v.at(lkey));
    }
    return dv;
}

double suHttpUtils::TruNum(const std::unordered_map<std::string, std::string>& k2v, const std::string& key)
{
    std::string lkey = TO_LOWER(key);
    if (k2v.find(lkey) != std::end(k2v))
    {
        return std::stod(k2v.at(lkey));
    }
    throw std::invalid_argument("缺少参数: " + key);
}