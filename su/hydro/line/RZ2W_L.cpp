/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RZ2W_L.c
 * @description: RZ2W_L实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "RZ2W_L.h"

RZ2W_L::RZ2W_L(const double& unit)
{
    m_unit = unit;
}
void RZ2W_L::AddData(const double& z, const double& w)
{
    m_Zs.push_back(z);
    m_Ws.push_back(w);
}
void RZ2W_L::SetData(const std::vector<double>& ZWs)
{
    Clear();
    if (ZWs.size()% 2 != 0 || ZWs.size() < 4)
    {
        throw std::invalid_argument("水位库容线设置错误");
    }
    int num = ZWs.size()/2;
    m_Zs.reserve(num);
    m_Ws.reserve(num);
    for (int i = 0; i < ZWs.size()/2; ++i)
    {
        m_Zs.push_back(ZWs[i * 2]);
        m_Ws.push_back(ZWs[i * 2 + 1]);
    }
}
void RZ2W_L::SetData(const std::vector<double>& Zs, const std::vector<double>& Ws)
{
    Clear();
    if (Zs.size() != Ws.size() || Zs.size() < 2)
    {
        throw std::invalid_argument("水位库容线设置错误");
    }
    int num = Zs.size();
    m_Zs.reserve(num);
    m_Ws.reserve(num);
    for (int i = 0; i < num; ++i)
    {
        m_Zs.push_back(Zs[i]);
        m_Ws.push_back(Ws[i]);
    }
}
void RZ2W_L::Clear()
{
    m_Zs.clear();
    m_Ws.clear();
}
void RZ2W_L::Sort()
{
    if (m_Zs.size() != m_Ws.size()) return; // 安全检查

    const size_t n = m_Zs.size();
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0); // indices = {0, 1, 2, ..., n-1}

    // 按 m_Zs 的值对索引排序
    std::sort(indices.begin(), indices.end(),
        [&](size_t i, size_t j) { return m_Zs[i] < m_Zs[j]; });

    // 创建排序后的新数组
    std::vector<double> sorted_Zs(n), sorted_Ws(n);
    for (size_t i = 0; i < n; ++i) {
        sorted_Zs[i] = m_Zs[indices[i]];
        sorted_Ws[i] = m_Ws[indices[i]];
    }

    // 替换原数组
    m_Zs = std::move(sorted_Zs);
    m_Ws = std::move(sorted_Ws);
}
double RZ2W_L::GetW(const double& z, const int& type) const
{
    const double w = AlgoL::Calc(z, m_Zs, m_Ws, type);
    return std::max(0.0,  w);
}
double RZ2W_L::GetZ(const double& w, const int& type) const
{
    const double z = AlgoL::Calc(w, m_Ws, m_Zs, type);
    return z;
}