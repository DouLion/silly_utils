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
    if (m_items.size() < 2)
    {
        return ret;
    }
    ret.m_items.reserve(length() / dist * 2);
    double remain = dist;
    ret.m_items.push_back(m_items.front());
    for (size_t i = 1; i < m_items.size(); ++i)
    {
        const suPoint& p0 = m_items[i - 1];
        const suPoint& p1 = m_items[i];
        double tmpDist = p1.dist(p0);
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
        ret.push_back(m_items.back());
    }
    return ret;
}

double suLine::length() const
{
    double ret = 0.0;
    if (m_items.size() < 2)
    {
        return ret;
    }
    for (size_t i = 1; i < m_items.size(); ++i)
    {
        ret += m_items[i].dist(m_items[i - 1]);
    }
    return ret;
}

bool suLine::intersect(const suLine& rh) const
{
    for (size_t i = 1; i < m_items.size(); ++i)
    {
        suSegment s1(m_items[i - 1], m_items[i]);
        for (size_t j = 1; j < rh.m_items.size(); ++j)
        {
            suSegment s2(rh.m_items[j-1], rh.m_items[j]);
            if (s1.intersect(s2))
            {
                return true;
            }
        }
    }
    return false;
}

std::vector<suPoint> suLine::intersection(const suLine& rh) const
{
    std::vector<suPoint> ret;
    for (size_t i = 1; i < m_items.size(); ++i)
    {
        suSegment s1(m_items[i - 1], m_items[i]);
        for (size_t j = 1; j < rh.m_items.size(); ++j)
        {
            suSegment s2(rh.m_items[j-1], rh.m_items[j]);
            std::optional<suPoint> tmp = s1.intersection(s2);
            if (tmp.has_value())
            {
                ret.push_back(tmp.value());
            }
        }
    }
    return ret;
}