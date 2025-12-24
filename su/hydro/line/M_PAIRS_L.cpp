/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: M_PAIRS_L
 * @description: M_PAIRS_L实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "M_PAIRS_L.h"
void MultiPairsL::AddLine(const double& k, const PairsL& pl)
{
    if (m_key2Pairs.find(k) != m_key2Pairs.end())
    {
        // 移除
        m_key2Pairs.erase(k);
    }
    m_key2Pairs[k] = pl;
}
double MultiPairsL::GetXFromY(const double& k, const double& y, const int& type) const
{
    PairsL tmp;
    for (const auto& [tk,  pl] : m_key2Pairs)
    {
        tmp.AddData(pl.GetXFromY(y, type), tk);
    }
    return tmp.GetXFromY(k, type);
}
double MultiPairsL::GetYFromX(const double& k, const double& x, const int& type) const
{
    PairsL tmp;
    for (const auto& [tk,  pl] : m_key2Pairs)
    {
        tmp.AddData(tk, pl.GetYFromX(x, type));
    }
    return tmp.GetYFromX(k, type);
}
double MultiPairsL::GetValFromXY(const double& x, const double& y, const int& type) const
{
    PairsL tmp;
    for (const auto& [tk,  pl] : m_key2Pairs)
    {
        tmp.AddData(tk, pl.GetXFromY(y, type));
    }
    return tmp.GetXFromY(x, type);
}
void MultiPairsL::Clear()
{
    m_key2Pairs.clear();
}