/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suLine.c
 * @description: suLine实现
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#include "silly_line.h"

suLine suLine::equidistant(const double& dist) const
{
    suLine ret;
    if (m_points.size() < 2)
    {
        return ret;
    }
    ret.m_points.reserve(distance() / dist * 2);
    double remain = dist;
    ret.m_points.push_back(m_points.front());
    for (size_t i = 1; i < m_points.size(); ++i)
    {
        const suPoint& p0 = m_points[i - 1];
        const suPoint& p1 = m_points[i];
        double tmpDist = p1.distance(p0);
        double ratio1 = dist / tmpDist;
        double ratio = remain / tmpDist;
        while (remain < tmpDist + 1e-8)
        {
            suPoint np;
            np.x = p0.x + ratio * (p1.x - p0.x);
            np.y = p0.y + ratio * (p1.y - p0.y);
            ret.push_back(np);
            ratio += ratio1;

            // 更新剩余长度
            tmpDist -= remain;
            remain = dist;
        }
        remain -= tmpDist;
    }
    // 最后一个点算完,剩余长度还较多,则添加最后一个点
    if (remain > 1e-8)
    {
        ret.push_back(m_points.back());
    }
    return ret;
}

double suLine::distance() const
{
    double ret = 0.0;
    if (m_points.size() < 2)
    {
        return ret;
    }
    for (size_t i = 1; i < m_points.size(); ++i)
    {
        ret += m_points[i].distance(m_points[i - 1]);
    }
    return ret;
}
suLineZ suLineZ::equidistant(const double& dist) const
{
    suLineZ ret;
    if (m_points.size() < 2)
    {
        return ret;
    }
    ret.m_points.reserve(distance() / dist * 2);
    double remain = dist;
    ret.m_points.push_back(m_points.front());
    for (size_t i = 1; i < m_points.size(); ++i)
    {
        const suPointZ& p0 = m_points[i - 1];
        const suPointZ& p1 = m_points[i];
        double tmpDist = p1.distance(p0);
        double ratio1 = dist / tmpDist;
        double ratio = remain / tmpDist;
        while (remain < tmpDist + 1e-8)
        {
            suPointZ np;
            np.x = p0.x + ratio * (p1.x - p0.x);
            np.y = p0.y + ratio * (p1.y - p0.y);
            np.z = p0.z + ratio * (p1.z - p0.z);
            ret.push_back(np);
            ratio += ratio1;

            // 更新剩余长度
            tmpDist -= remain;
            remain = dist;
        }
        remain -= tmpDist;
    }
    // 最后一个点算完,剩余长度还较多,则添加最后一个点
    if (remain > 1e-8)
    {
        ret.push_back(m_points.back());
    }
    return ret;
}
double suLineZ::distance() const
{
    double ret = 0.0;
    if (m_points.size() < 2)
    {
        return ret;
    }
    for (size_t i = 1; i < m_points.size(); ++i)
    {
        ret += m_points[i].distance(m_points[i - 1]);
    }
    return ret;
}