/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suSegment.c
 * @description: suSegment实现
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#include "silly_segment.h"
bool suSegment::intersect(const suSegment& rh) const
{
    const suPoint& a = p0;
    const suPoint& b = p1;
    const suPoint& c = rh.p0;
    const suPoint& d = rh.p1;
    double dx1 = b.x - a.x, dy1 = b.y - a.y;
    double dx2 = d.x - c.x, dy2 = d.y - c.y;

    double det = dx1 * dy2 - dy1 * dx2;

    // 平行或重合
    if (det == 0) {
        return false;  // 简化处理，实际需要判断共线重叠
    }

    double t = ((a.x - c.x) * dy2 - (a.y - c.y) * dx2) / det;
    double u = -((a.x - c.x) * dy1 - (a.y - c.y) * dx1) / det;

    // 检查参数是否在 [0,1] 范围内（包括端点）
    return (t >= 0 && t <= 1) && (u >= 0 && u <= 1);
}
std::optional<suPoint> suSegment::intersection(const suSegment& rh) const
{
    double x1 = p0.x;
    double y1 = p0.y;
    double x2 = p1.x;
    double y2 = p1.y;

    double x3 = rh.p0.x;
    double y3 = rh.p0.y;
    double x4 = rh.p1.x;
    double y4 = rh.p1.y;

    // 计算分母
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // 平行或重合
    if (std::fabs(denominator) < SU_TINY)
    {
        return std::nullopt;
    }

    // 计算交点的参数 t 和 u
    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
    double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;

    // 检查交点是否在线段内
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        double intersectX = x1 + t * (x2 - x1);
        double intersectY = y1 + t * (y2 - y1);
        return suPoint(intersectX, intersectY);
    }

    // 交点不在两条线段上
    return std::nullopt;
}
double suSegment::distance(const suPoint& p, const bool& in_seg) const
{
    // 如果线段退化为点
    if (p0.x == p1.x && p0.y == p1.y) {
        return p.dist(p0);
    }
    // 向量表示
    double dx = p1.x - p0.x;
    double dy = p1.y - p0.y;
    // 计算投影参数 t
    double t = ((p.x - p0.x) * dx + (p.y - p0.y) * dy) / (dx * dx + dy * dy);
    // 限制 t 在线段范围内
    if (in_seg)
    {
        t = std::max(0.0, std::min(1.0, t));
    }

    // 计算投影点坐标
    suPoint projection;
    projection.x = p0.x + t * dx;
    projection.y = p0.y + t * dy;

    // 返回点到投影点的距离
    return p.dist(projection);
}