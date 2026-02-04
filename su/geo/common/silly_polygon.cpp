/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_polygon.c
 * @description: silly_polygon实现
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#include "silly_polygon.h"
double suRing::area() const
{
    return SU_CLOSED_RING_NORMAL_AREA(points);
}

suRect suRing::bound() const
{
    suRect ret;
    for (const suPoint& point : points)
    {
        ret.min.x = std::min(ret.min.x, point.x);
        ret.min.y = std::min(ret.min.y, point.y);
        ret.max.x = std::max(ret.max.x, point.x);
        ret.max.y = std::max(ret.max.y, point.y);
    }
    return ret;
}
bool suRing::intersect(const suPoint& p) const
{
    return SU_POINT_IN_CLOSED_RING(p, points);
}

double suPoly::area() const
{
    double outerArea = outer.area();
    double holeArea = 0;
    for (const auto& ring : holes)
    {
        holeArea += ring.area();
    }
    return outerArea - holeArea;
}

suRect suPoly::bound() const
{
    return outer.bound();
}

bool suPoly::intersect(const suPoint& p) const
{
    if (!outer.intersect(p))
    {
        return false;
    }
    for (const auto& ring : holes)
    {
        if (ring.intersect(p))
        {
            return false;
        }
    }
    return true;
}

double suMultiPoly::area() const
{
    double total = 0;
    for (const auto& poly : m_items)
    {
        total += poly.area();
    }
    return total;
}

suRect suMultiPoly::bound() const
{
    suRect ret = m_items.front().bound();
    for (size_t i = 1; i < m_items.size(); ++i)
    {
        suRect tmp = m_items[i].bound();
        ret = ret.MBR(tmp);
    }
    return ret;
}

bool suMultiPoly::intersect(const suPoint& p) const
{
    for (const auto poly : m_items)
    {
        if (poly.intersect(p))
        {
            return true;
        }
    }
    return false;
}