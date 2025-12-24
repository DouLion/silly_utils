/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: PAIRS_L
 * @description: PAIRS_L实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "PAIRS_L.h"
PairsL::PairsL(const double& xScale)
{
}
void PairsL::AddData(const double& x, const double& y)
{
    m_Xs.push_back(x);
    m_Ys.push_back(y);
}
void PairsL::SetData(const std::vector<double>& ZWs)
{
    Clear();
    if (ZWs.size() % 2 != 0 || ZWs.size() < 4)
    {
        throw std::invalid_argument("xy关系线设置错误");
    }
    int num = ZWs.size() / 2;
    m_Xs.reserve(num);
    m_Ys.reserve(num);
    for (int i = 0; i < ZWs.size() / 2; ++i)
    {
        m_Xs.push_back(ZWs[i * 2]);
        m_Ys.push_back(ZWs[i * 2 + 1]);
    }
}
void PairsL::SetData(const std::vector<double>& Zs, const std::vector<double>& Ws)
{
    Clear();
    if (Zs.size() != Ws.size() || Zs.size() < 2)
    {
        throw std::invalid_argument("xy关系线设置错误");
    }
    int num = Zs.size();
    m_Xs.reserve(num);
    m_Ys.reserve(num);
    for (int i = 0; i < num; ++i)
    {
        m_Xs.push_back(Zs[i]);
        m_Ys.push_back(Ws[i]);
    }
}
void PairsL::Clear()
{
    m_Xs.clear();
    m_Ys.clear();
}
void PairsL::SortByX()
{
    if (m_Xs.size() != m_Ys.size())
        return;  // 安全检查

    const size_t n = m_Xs.size();
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);  // indices = {0, 1, 2, ..., n-1}

    // 按 m_Xs 的值对索引排序
    std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) { return m_Xs[i] < m_Xs[j]; });

    // 创建排序后的新数组
    std::vector<double> sorted_Zs(n), sorted_Ws(n);
    for (size_t i = 0; i < n; ++i)
    {
        sorted_Zs[i] = m_Xs[indices[i]];
        sorted_Ws[i] = m_Ys[indices[i]];
    }

    // 替换原数组
    m_Xs = std::move(sorted_Zs);
    m_Ys = std::move(sorted_Ws);
}
void PairsL::SortByY()
{
    if (m_Xs.size() != m_Ys.size())
        return;  // 安全检查

    const size_t n = m_Xs.size();
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);  // indices = {0, 1, 2, ..., n-1}

    // 按 m_Xs 的值对索引排序
    std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) { return m_Ys[i] < m_Ys[j]; });

    // 创建排序后的新数组
    std::vector<double> sorted_Zs(n), sorted_Ws(n);
    for (size_t i = 0; i < n; ++i)
    {
        sorted_Zs[i] = m_Xs[indices[i]];
        sorted_Ws[i] = m_Ys[indices[i]];
    }

    // 替换原数组
    m_Xs = std::move(sorted_Zs);
    m_Ys = std::move(sorted_Ws);
}
double PairsL::GetMaxX() const
{
    return *std::max_element(m_Xs.begin(), m_Xs.end());
}
double PairsL::GetMaxY() const
{
    return *std::max_element(m_Ys.begin(), m_Ys.end());
}
double PairsL::GetYFromX(const double& x, const int& type) const
{
    const double y = AlgoL::Calc(x, m_Xs, m_Ys, type);
    return std::max(0.0, y);
}
double PairsL::GetXFromY(const double& y, const int& type) const
{
    const double x = AlgoL::Calc(y, m_Ys, m_Xs, type);
    return x;
}