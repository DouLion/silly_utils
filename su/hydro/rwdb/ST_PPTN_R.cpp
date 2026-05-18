/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ST_PPTN_R.c
 * @description: ST_PPTN_R实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "ST_PPTN_R.h"
#include "system/silly_system.h"
double RWDB::ST_PPTN_R::INTV2S(const double& intv)
{
    if (intv <= 0.0)
    {
        return 1.0;
    }
    double hour = std::floor(intv);
    double minute = intv - hour;
    return hour * 3600 + minute * 60;
}
double RWDB::ST_PPTN_R::INTV2MS(const double& intv)
{
    return INTV2S(intv) * 1000.0;
}

std::map<std::time_t, double> RWDB::ST_PPTN_R::Resample(const std::map<std::time_t, double>& tm2rain, const std::time_t& bt, const std::time_t& et, const std::time_t& intv)
{
    auto t = bt +  intv;
    std::map<std::time_t, double> ret;
    auto it = tm2rain.begin();
    auto eit = tm2rain.end();
    while (it != eit && it->first <= bt)
    {
        ++it;
    }
    while (t <= et)
    {
        float v = 0;
        while (it!=eit && it->first<=t)
        {
            v+=it->second;
            ++it;
        }
        ret[t] = v;
        t+=intv;
    }
    return ret;
}

std::map<std::string, std::string> RWDB::ST_PPTN_R::MaxDrpStation(const std::map<std::string, std::vector<std::string>>& code2stcds, const std::map<std::string, std::map<std::time_t, float>>& stcd2tm2drp)
{
    std::map<std::string, std::string> ret;
    std::map<std::string, float> stcd2sum;
    for (const auto& [stcd, tm2drp] : stcd2tm2drp )
    {
        float tmp = 0;
        for (const auto& [_, drp]: tm2drp)
        {
            tmp+= drp;
        }
        stcd2sum[stcd] = tmp;
    }
    for (const auto& [code, stcds] : code2stcds)
    {
        float maxSum = 0;
        std::string maxStcd;
        for (const auto & stcd: stcds)
        {
            if (HAS(stcd2sum, stcd))
            {
                if (const float& tsum = stcd2sum[stcd]; tsum > maxSum)
                {
                    maxSum = tsum;
                    maxStcd = stcd;
                }
            }
        }
        if (maxStcd.empty())
        {
            continue;
        }
        ret[code] = maxStcd;
    }
    return ret;

}