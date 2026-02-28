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

const double EPSILON = 1e-10;
// 辅助函数：浮点数近似相等比较
bool is_nearly_equal(double a, double b)
{
    return std::abs(a - b) < EPSILON;
}

// 辅助函数：判断点 p 是否在线段 (a, b) 上
// 这包含了共线性检查和点是否在线段的包围盒内检查
bool is_on_segment(const suPoint& p, const suPoint& a, const suPoint& b)
{
    // 1. 检查共线性：使用叉积，如果点在线段延长线上，叉积为0
    // (p - a) x (b - a) = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x)
    double cross_product = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    if (!is_nearly_equal(cross_product, 0.0))
    {
        return false;  // 不共线
    }

    // 2. 检查点是否在线段的包围盒内（包括端点）
    // 即检查 p.x 是否在 [min(a.x, b.x), max(a.x, b.x)] 之间
    // 和 p.y 是否在 [min(a.y, b.y), max(a.y, b.y)] 之间
    bool x_in_bounds = (p.x >= std::min(a.x, b.x) - EPSILON && p.x <= std::max(a.x, b.x) + EPSILON);
    bool y_in_bounds = (p.y >= std::min(a.y, b.y) - EPSILON && p.y <= std::max(a.y, b.y) + EPSILON);

    return x_in_bounds && y_in_bounds;
}
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

extern std::vector<suPoint> SU_SMOOTH_B_SPLINE(const std::vector<suPoint>& points, int density_factor)
{
    if (points.empty()) {
        throw std::invalid_argument("Input points cannot be empty");
    }

    size_t n = points.size();
    if (n == 1) return points; // 单点直接返回
    if (density_factor < 1) density_factor = 1;

    std::vector<suPoint> result;
    // 预分配内存：n 段 * (density + 1) 个点
    result.reserve(n * (density_factor + 1));

    // --- 2. 核心循环 ---
    // 遍历每一个点作为当前段的起始参考点
    // 均匀 B 样条的每一段由 P[i-1], P[i], P[i+1], P[i+2] 决定
    for (size_t i = 0; i < n; ++i) {
        // 使用模运算获取 4 个控制点，自动处理首尾循环
        // 这种写法同时兼容了“视觉上的闭合”和“开放曲线的自然边界”
        const suPoint& p0 = points[(i + n - 1) % n]; // P(i-1)
        const suPoint& p1 = points[i];               // P(i)
        const suPoint& p2 = points[(i + 1) % n];     // P(i+1)
        const suPoint& p3 = points[(i + 2) % n];     // P(i+2)

        // 在当前段 P1 -> P2 之间进行插值
        for (int k = 0; k <= density_factor; ++k) {
            // 【去重优化】如果不是最后一段，跳过当前段的最后一个点 (t=1.0)
            // 因为 t=1.0 的点等同于下一段 t=0.0 的点
            bool is_last_segment = (i == n - 1);
            if (!is_last_segment && k == density_factor) {
                continue;
            }

            double t = static_cast<double>(k) / density_factor;
            double t2 = t * t;
            double t3 = t2 * t;

            // --- 三次均匀 B 样条基函数 (权重 1/6) ---
            // B0 = (1-t)^3
            // B1 = 3t^3 - 6t^2 + 4
            // B2 = -3t^3 + 3t^2 + 3t + 1
            // B3 = t^3
            double b0 = (1.0 - t) * (1.0 - t) * (1.0 - t);
            double b1 = 3.0 * t3 - 6.0 * t2 + 4.0;
            double b2 = -3.0 * t3 + 3.0 * t2 + 3.0 * t + 1.0;
            double b3 = t3;

            constexpr double w = 1.0 / 6.0;

            double px = w * (b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x);
            double py = w * (b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y);

            result.push_back({px, py});
        }
    }

    return result;
}