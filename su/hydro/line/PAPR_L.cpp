/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: PAPR_L
 * @description: PAPR_L实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "PAPR_L.h"
double PaPR_L::GetMaxPa() const
{
    if (m_key2Pairs.empty())
    {
        return 0;
    }
    return m_key2Pairs.rbegin()->first;
}
double PaPR_L::GetR(const double& pa, const double& p) const
{
    if (m_key2Pairs.empty())
    {
        return 0;
    }
    if (m_key2Pairs.size() == 1)
    {
        double r =  m_key2Pairs.begin()->second.GetYFromX(pa+ p, 2);
        return std::min(std::max(r, 0.0), p);
    }
    double r = GetYFromX(pa, p, 2);
    return std::min(r, p);
}
double PaPR_L::GetPA(const double& p, const double& r) const
{
    if (m_key2Pairs.empty())
    {
        return 0;
    }
    if (m_key2Pairs.size() == 1)
    {
        double pa = m_key2Pairs.begin()->second.GetXFromY(r, 2) - p;
        return std::max(pa, 0.0);
    }
    PairsL tmp;
    for (const auto& [k,  pl]: m_key2Pairs)
    {
        tmp.AddData(k, -pl.GetXFromY(r, 2));
    }
    return tmp.GetXFromY(-p, 2);
}
double PaPR_L::GetP(const double& pa, const double& r, const double& Wm) const
{
    if (m_key2Pairs.empty())
    {
        return 0;
    }
    const double ratio = 1.1;
    if (m_key2Pairs.size() == 1 && m_key2Pairs.begin()->second.m_Xs.size() > 1)
    {
        auto& line = m_key2Pairs.begin()->second;
        if (pa > line.m_Xs.front() && pa < Wm)// 蓄满产流处理
        {
            const double tmpPA = line.m_Xs.front();
            const double addP = std::max(Wm - pa, 0.0);
            return line.GetXFromY(r, 2) - tmpPA + addP;
        }
        // 常规计算
        double pVal = line.GetXFromY(r, 2) - pa;
        return std::max(pVal, ratio * r);
    }
    return std::max(GetXFromY(pa, r, 2), r);
}