/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-06-05
 * @file: silly_rain_utils.cpp
 * @description: silly_rain_utils实现
 * @version: v1.0.1 2025-06-05 dou li yang
 */
#include "silly_rain_utils.h"
using namespace su;
std::map<std::string, double> RainUtils::adjust_weight(const std::map<std::string, double>& stcd2weight, const std::map<std::string, double>& stcd2rain, const double& ignore)
{
    std::map<std::string, double> ret;

    double sumW = 0.0;
    for (const auto& [stcd, rain] : stcd2rain)
    {
        if (rain < ignore && stcd2weight.find(stcd) != stcd2weight.end())
        {
            sumW += stcd2weight.at(stcd);
            ret[stcd] = stcd2weight.at(stcd);
        }
    }

    for (auto& [stcd, weight] : ret)
    {
        weight = weight / sumW;
    }

    return ret;
}
double RainUtils::weight_rain(const std::map<std::string, double>& stcd2weight, const std::map<std::string, double>& stcd2rain)
{
    double ret = 0.0;
    for (const auto& [stcd, weight] : stcd2weight)
    {
        if (stcd2rain.find(stcd) == stcd2rain.end())
        {
            continue;
        }
        ret += weight * stcd2rain.at(stcd);
    }
    return ret;
}
