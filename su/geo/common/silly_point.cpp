/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suPoint.c
 * @description: suPoint实现
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#include "silly_point.h"

extern bool SU_POINT_IN_CLOSED_RING(const suPoint& point, const std::vector<suPoint>& ring)
{
    bool is_inside = false;
    const size_t pnum = ring.size();
    if (pnum < 3)
    {
        return false;  // 至少需要3个点构成一个三角形
    }


    for (size_t i = 0, j = pnum - 1; i < pnum; j = i++)
    {
        const auto& v1 = ring[j];
        const auto& v2 = ring[i];

        // 检查点是否在线段上（避免浮点精度问题）
        const double dx = v2.x - v1.x;
        const double dy = v2.y - v1.y;
        if (std::abs(dy) > std::numeric_limits<double>::epsilon())
        {
            const double intersection_x = (dx * (point.y - v1.y) / dy) + v1.x;
            if (std::abs(point.x - intersection_x) < std::numeric_limits<double>::epsilon())
            {
                return false;  // 点在边界上
            }
        }

        // 射线法判断内外
        if (((v1.y <= point.y && point.y < v2.y) || (v2.y <= point.y && point.y < v1.y)) && (point.x < (dx * (point.y - v1.y) / dy + v1.x)))
        {
            is_inside = !is_inside;
        }
    }
    return is_inside;
}

extern double SU_CLOSED_RING_ORIENTED_AREA(const std::vector<suPoint>& ring)
{
    double result = 0.0;
    size_t pnum = ring.size();
    if (pnum < 3)
    {
        return 0.0;
    }

    for (size_t i = 0; i < pnum; ++i)
    {
        size_t j = (i + 1) % pnum;
        result += ring[i].x * ring[j].y;
        result -= ring[j].x * ring[i].y;
    }
    return result / 2.0;
}

extern double SU_CLOSED_RING_NORMAL_AREA(const std::vector<suPoint>& ring)
{
    return std::abs(SU_CLOSED_RING_ORIENTED_AREA(ring));
}