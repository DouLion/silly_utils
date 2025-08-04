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
static const std::unordered_map<std::string, eMobileOP> MOBILE_PREFIX_MAP = {
    {"138", eMobileOP::ChinaMobile},
    {"139", eMobileOP::ChinaMobile},
    {"150", eMobileOP::ChinaMobile},
    {"159", eMobileOP::ChinaMobile},
    {"135", eMobileOP::ChinaMobile},
    {"186", eMobileOP::ChinaUnicom},
    {"180", eMobileOP::ChinaTelecom},
    {"133", eMobileOP::ChinaTelecom},
    {"156", eMobileOP::ChinaTelecom},
    {"181", eMobileOP::ChinaTelecom}
    // 可以继续扩展更多号段
};

std::string mobile::op2str(const eMobileOP& op)
{
    switch (op)
    {
        case eMobileOP::ChinaMobile:
            return "China Mobile";
        case eMobileOP::ChinaUnicom:
            return "China Unicom";
        case eMobileOP::ChinaTelecom:
            return "China Telecom";
        default:
            return "Unknown";
    }
}
eMobileOP mobile::opcode(const std::string& number)
{
    std::string prefix = number.substr(0, 3);
    auto iter = MOBILE_PREFIX_MAP.find(prefix);
    // 查找对应的运营商
    if (iter != MOBILE_PREFIX_MAP.end())
    {
        return iter->second;
    }
    return eMobileOP::Unknown;
}
std::map<std::string, eMobileOP> mobile::opcode(const std::vector<std::string>& numbers)
{
    std::map<std::string, eMobileOP> ret;
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
