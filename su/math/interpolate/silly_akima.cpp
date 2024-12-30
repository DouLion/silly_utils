/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-30
 * @file: silly_akima.cpp
 * @description: silly_akima实现
 * @version: v1.0.1 2024-12-30 dou li yang
 */
#include "silly_akima.h"
using namespace silly::interpolation;

std::vector<double> akima::derivatives(const std::vector<double>& xx, const std::vector<double>& yy)
{
    //
    if (xx.size() != yy.size())
    {
        throw std::runtime_error("数据长度不一致");
    }
    if (xx.size() < 4)
    {
        throw std::runtime_error("至少需要4组数");
    }
    m_x = xx;
    m_y = yy;
    slopes();
    // 端点导数采用邻近斜率的平均
    auto n = m_slopes.size();
    m_derivatives.resize(m_x.size());
    m_derivatives[0] = m_slopes[0];
    m_derivatives[n - 1] = m_slopes[n - 2];

    for (size_t i = 1; i < n - 1; ++i)
    {
        double w1 = std::abs(m_slopes[i] - m_slopes[i - 1]);
        double w2 = std::abs(m_slopes[i + 1] - m_slopes[i - 1]);
        if (w1 + w2 != 0)
        {
            m_derivatives[i] = (w1 * m_slopes[i + 1] + w2 * m_slopes[i - 1]) / (w1 + w2);
        }
        else
        {
            m_derivatives[i] = 0.5 * (m_slopes[i - 1] + m_slopes[i]);
        }
    }
    return m_derivatives;
}
double akima::calc(const double& x) const
{
    size_t n = m_x.size();
    if (x <= m_x[0])
    {
        return m_y[0];  // 如果 x 小于等于第一个点，返回第一个点的值
    }
    else if (x >= m_x[n - 1])
    {
        return m_y[n - 1];  // 如果 x 大于等于最后一个点，返回最后一个点的值
    }
    size_t i = 0;
    while (i < n - 1 && x > m_x[i + 1])
        i++;

    double h = m_x[i + 1] - m_x[i];
    double t = (x - m_x[i]) / h;
    double t2 = t * t;
    double t3 = t2 * t;

    // 三次多项式
    double a = 2 * t3 - 3 * t2 + 1;
    double b = t3 - 2 * t2 + t;
    double c = -2 * t3 + 3 * t2;
    double d = t3 - t2;

    return a * m_y[i] + b * h * m_derivatives[i] + c * m_y[i + 1] + d * h * m_derivatives[i + 1];
}
void akima::slopes()
{
    size_t n = m_x.size();
    m_slopes.resize(n - 1);
    for (size_t i = 0; i < n - 1; ++i)
    {
        m_slopes[i] = (m_y[i + 1] - m_y[i]) / (m_x[i + 1] - m_x[i]);
    }
}
std::vector<double> akima::derivatives(const std::vector<std::pair<double, double>>& posize_ts)
{
    m_x.resize(posize_ts.size());
    m_y.resize(posize_ts.size());
    for (size_t i = 0; i < posize_ts.size(); ++i)
    {
        m_x[i] = posize_ts[i].first;
        m_y[i] = posize_ts[i].second;
    }
    return derivatives(m_x, m_y);
}
