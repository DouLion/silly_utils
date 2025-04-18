/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-04-18
 * @file: silly_mobile.cpp
 * @description: silly_mobile实现
 * @version: v1.0.1 2025-04-18 dou li yang
 */
#include "silly_mobile.h"
using namespace silly;
static const std::unordered_map<std::string, mobile::eop> MOBILE_PREFIX_MAP = {
    {"138", mobile::eop::ChinaMobile},
    {"139", mobile::eop::ChinaMobile},
    {"150", mobile::eop::ChinaMobile},
    {"159", mobile::eop::ChinaMobile},
    {"135", mobile::eop::ChinaMobile},
    {"186", mobile::eop::ChinaUnicom},
    {"180", mobile::eop::ChinaTelecom},
    {"133", mobile::eop::ChinaTelecom},
    {"156", mobile::eop::ChinaTelecom},
    {"181", mobile::eop::ChinaTelecom}
    // 可以继续扩展更多号段
};

std::string mobile::op2str(const mobile::eop& op)
{
    switch (op)
    {
        case mobile::eop::ChinaMobile:
            return "China Mobile";
        case mobile::eop::ChinaUnicom:
            return "China Unicom";
        case mobile::eop::ChinaTelecom:
            return "China Telecom";
        default:
            return "Unknown";
    }
}
mobile::eop mobile::opcode(const std::string& number)
{
    std::string prefix = number.substr(0, 3);
    auto iter = MOBILE_PREFIX_MAP.find(prefix);
    // 查找对应的运营商
    if (iter != MOBILE_PREFIX_MAP.end())
    {
        return iter->second;
    }
    return mobile::eop::Unknown;
}
std::map<std::string, mobile::eop> mobile::opcode(const std::vector<std::string>& numbers)
{
    std::map<std::string, mobile::eop> ret;
    for (const auto& number : numbers)
    {
        ret[number] = opcode(number);
    }
    return ret;
}
std::string mobile::opstr(const std::string& number)
{
    return op2str(opcode(number));
}
std::map<std::string, std::string> mobile::opstr(const std::vector<std::string>& numbers)
{
    std::map<std::string, std::string> ret;
    for (const auto& number : numbers)
    {
        ret[number] = opstr(number);
    }
    return ret;
}
