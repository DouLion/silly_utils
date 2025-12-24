/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RZ2F_L
 * @description: RZ2F_L实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "RZ2F_L.h"
double RZ2F_L::GetF(const double& e, const double& z) const
{
    return GetYFromX(e, z);
}
double RZ2F_L::GetZ(const double& e, const double& f) const
{
    return GetXFromY(e, f);;
}
double RZ2F_L::GetE(const double& z, const double& f) const
{
    return GetValFromXY(z, f);;
}
double RZ2F_L::GetMaxE() const
{
    if (m_key2Pairs.empty()) return 0;
    return m_key2Pairs.rbegin()->first;
}
double RZ2F_L::GetMaxZ() const
{
    if (m_key2Pairs.empty()) return 0;
    double ret = -1e12;
    for (const auto& [_, l] : m_key2Pairs)
    {
        ret = std::max(ret, l.GetMaxX());
    }
    return ret;
}
double RZ2F_L::GetMaxF() const
{
    if (m_key2Pairs.empty()) return 0;
    double ret = -1e12;
    for (const auto& [_, l] : m_key2Pairs)
    {
        ret = std::max(ret, l.GetMaxY());
    }
    return ret;
}