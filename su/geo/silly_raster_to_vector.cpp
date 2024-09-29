//
// Created by dell on 2023/12/6.
//

#include "silly_raster_to_vector.h"
#include <log/silly_log.h>
#include <vacuate/silly_vacuate.h>

#define RECURSION_TRACE_LINE(r, c)                               \
    if (r > -1 && c > -1 && r < m_height + 2 && c < m_width + 2) \
    {                                                            \
        for (auto &segment : m_mat[r][c].segments)               \
        {                                                        \
            if (!segment.traced)                                 \
            {                                                    \
                if (ring.points.back() == segment.f)             \
                {                                                \
                    ring.points.push_back(segment.t);            \
                    segment.traced = 1;                          \
                    trace_one_line(r, c, ring);                  \
                    return;                                      \
                }                                                \
                if (ring.points.back() == segment.t)             \
                {                                                \
                    ring.points.push_back(segment.f);            \
                    segment.traced = 1;                          \
                    trace_one_line(r, c, ring);                  \
                    return;                                      \
                }                                                \
            }                                                    \
        }                                                        \
    }

#define CALC_LINE_0_INTER(m, r, c, p) interpolation(m[r][c], m[r][c + 1], p)
#define CALC_LINE_1_INTER(m, r, c, p) interpolation(m[r][c + 1], m[r + 1][c + 1], p)
#define CALC_LINE_2_INTER(m, r, c, p) interpolation(m[r + 1][c], m[r + 1][c + 1], p)
#define CALC_LINE_3_INTER(m, r, c, p) interpolation(m[r][c], m[r + 1][c], p)

void silly_vectorizer::interpolation(const silly_trace_node &n1, const silly_trace_node &n2, silly_point &point)
{
    switch (m_interpolation_mode)
    {
        case 1:  // 线性插值
                 ///  !!!! 经度和纬度一定有一个相等 !!!!
#ifndef NDEBUG
            if (n1.p.x == n2.p.x)
            {
                point.x = n1.p.x;

                point.y = n1.p.y + m_ydelta * (m_threshold_l - n1.val) / (n2.val - n1.val);
            }
            else if (n1.p.y == n2.p.y)
            {
                point.y = n1.p.y;
                point.x = n1.p.x + m_xdelta * (m_threshold_l - n1.val) / (n2.val - n1.val);
            }
            else
            {
                SLOG_ERROR("\n{:02f}, {:02f}\n{:02f}, {:02f}", n1.p.x, n1.p.y, n2.p.x, n2.p.y)
                throw std::runtime_error("经纬度必须相等");
            }
#else
            if (n1.p.x == n2.p.x)
            {
                point.x = n1.p.x;

                point.y = n1.p.y + m_ydelta * (m_threshold_l - n1.val) / (n2.val - n1.val);
            }
            else
            {
                point.y = n1.p.y;
                point.x = n1.p.x + m_xdelta * (m_threshold_l - n1.val) / (n2.val - n1.val);
            }
#endif
            break;
        case 2:  // 直接取中点
            point.x = (n1.p.x + n2.p.x) / 2.;
            point.y = (n1.p.y + n2.p.y) / 2.;

        default:
            break;
    }
}

bool silly_vectorizer::recurse_trace_line(int r, int c, silly_ring &ring)
{
    if (r > -1 && c > -1 && r < m_height + 2 && c < m_width + 2)
    {
        for (auto &segment : m_mat[r][c].segments)
        {
            if (!segment.traced)
            {
                if (ring.points.back() == segment.f)
                {
                    ring.points.push_back(segment.t);
                    segment.traced = 1;
                    return true;
                }
                if (ring.points.back() == segment.t)
                {
                    ring.points.push_back(segment.f);
                    segment.traced = 1;
                    return true;
                }
            }
        }
    }
    return false;
}

/*void silly_vectorizer::trace_one_line(int r0l, int c0l, silly_ring &ring)
{
    // 上
    int r = r0l - 1;
    int c = c0l;
    RECURSION_TRACE_LINE(r, c)
    // 右
    r = r0l;
    c = c0l + 1;
    RECURSION_TRACE_LINE(r, c)
    // 下
    r = r0l + 1;
    c = c0l;
    RECURSION_TRACE_LINE(r, c)
    // 左
    r = r0l;
    c = c0l - 1;
    RECURSION_TRACE_LINE(r, c)
}*/

void silly_vectorizer::trace_one_line(int r0l, int c0l, silly_ring &ring)
{
    bool has_next = true;

    int nrow = r0l, ncol = c0l;
    while (has_next)
    {
        // 上
        int tmpr = nrow - 1;
        int tmpc = ncol;
        if (recurse_trace_line(tmpr, tmpc, ring))
        {
            nrow = tmpr;
            ncol = tmpc;
            continue;
        }
        // 右
        tmpr = nrow;
        tmpc = ncol + 1;
        if (recurse_trace_line(tmpr, tmpc, ring))
        {
            nrow = tmpr;
            ncol = tmpc;
            continue;
        }
        // 下
        tmpr = nrow + 1;
        tmpc = ncol;
        if (recurse_trace_line(tmpr, tmpc, ring))
        {
            nrow = tmpr;
            ncol = tmpc;
            continue;
        }
        // 左
        tmpr = nrow;
        tmpc = ncol - 1;
        if (recurse_trace_line(tmpr, tmpc, ring))
        {
            nrow = tmpr;
            ncol = tmpc;
            continue;
        }
        has_next = false;
    }
    // assert(ring.points.size() > 2);
    return;
}

bool silly_vectorizer::point_in_ring(const silly_point &p, const silly_ring &ring, const double &maxx)
{
    /* OGRPoint* po = geo_utils::SillyPointToOGRPoint(p);

     OGRPolygon* pl =  geo_utils ::SillyRingToPolygon(ring);
     return pl->Intersects(po);*/

    int n = ring.points.size();
    if (n < 3)
        return false;  // 多边形至少需要三个顶点

    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        const silly_point &a = ring.points[i];
        const silly_point &b = ring.points[j];

        // 检查线段 (a, b) 是否与射线交叉
        if ((a.y > p.y) != (b.y > p.y) && (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x))
        {
            inside = !inside;  // 每次交点切换inside状态
        }
    }
    return inside;
}

std::vector<silly_poly> silly_vectorizer::trace_all_polys()
{
    std::vector<silly_poly> result;
    bool has_not_traced = true;
    int count = 0;
    std::vector<silly_ring> all_rings;
    while (has_not_traced)
    {
        has_not_traced = false;
        silly_ring ring;
        silly_point mark_point;  // TODO:
        // 检查第一个未被追踪到的点
        for (int r = 1; r < m_height + 2 && !has_not_traced; ++r)
        {
            for (int c = 1; c < m_width + 2 && !has_not_traced; ++c)
            {
                auto &tmp = m_mat[r][c];
                switch (tmp.cv)
                {
                    case 0:
                    case 15:
                        break;
                    default:
                        for (auto &segment : tmp.segments)
                        {
                            if (!segment.traced)
                            {
                                // SLOG_DEBUG("起始点\n{},{}\n{},{}", segment.f.x, segment.f.y, segment.t.x, segment.t.y)
                                ring.points.push_back(segment.f);
                                ring.points.push_back(segment.t);
                                segment.traced = 1;

                                // 递归查找下一个点
                                trace_one_line(r, c, ring);
                                // 计算闭合凸多边形的形心,保证检查点一定落在阈值之上的面内
                                switch (tmp.cv)
                                {
                                    case 1:
                                        mark_point.x = (tmp.p.x + segment.f.x + segment.t.x) / 3.0;
                                        mark_point.y = (tmp.p.y - m_ydelta + segment.f.y + segment.t.y) / 3.0;
                                        break;
                                    case 2:
                                        mark_point.x = (tmp.p.x + m_xdelta + segment.f.x + segment.t.x) / 3.0;
                                        mark_point.y = (tmp.p.y - m_ydelta + segment.f.y + segment.t.y) / 3.0;
                                        break;
                                    case 3:
                                        mark_point.x = (tmp.p.x * 2 + m_xdelta + segment.f.x + segment.t.x) / 4.0;
                                        mark_point.y = (tmp.p.y * 2 - m_ydelta * 2 + segment.f.y + segment.t.y) / 4.0;
                                        break;
                                    case 4:
                                        mark_point.x = (tmp.p.x + m_xdelta + segment.f.x + segment.t.x) / 3.0;
                                        mark_point.y = (tmp.p.y + segment.f.y + segment.t.y) / 3.0;
                                        break;
                                    case 5:
                                        //
                                        mark_point.x = (tmp.p.x * 2 + m_xdelta + tmp.segments[0].f.x + tmp.segments[0].t.x + tmp.segments[1].f.x + tmp.segments[1].t.x) / 6.0;
                                        mark_point.y = (tmp.p.y * 2 - m_ydelta + tmp.segments[0].f.y + tmp.segments[0].t.y + tmp.segments[1].f.y + tmp.segments[1].t.y) / 6.0;
                                        break;
                                    case 6:
                                        mark_point.x = (tmp.p.x * 2 + m_xdelta * 2 + segment.f.x + segment.t.x) / 4.0;
                                        mark_point.y = (tmp.p.y * 2 - m_ydelta + segment.f.y + segment.t.y) / 4.0;
                                        break;
                                    case 7:
                                        mark_point.x = (tmp.p.x * 3 + m_xdelta * 2 + segment.f.x + segment.t.x) / 5.0;
                                        mark_point.y = (tmp.p.y * 3 - m_ydelta * 2 + segment.f.y + segment.t.y) / 5.0;
                                        break;
                                    case 8:
                                        mark_point.x = (tmp.p.x + segment.f.x + segment.t.x) / 3.0;
                                        mark_point.y = (tmp.p.y + segment.f.y + segment.t.y) / 3.0;
                                        break;
                                    case 9:
                                        mark_point.x = (tmp.p.x * 2 + segment.f.x + segment.t.x) / 4.0;
                                        mark_point.y = (tmp.p.y * 2 - m_ydelta + segment.f.y + segment.t.y) / 4.0;
                                        break;
                                    case 10:
                                        //
                                        mark_point.x = (tmp.p.x * 2 + m_xdelta + tmp.segments[0].f.x + tmp.segments[0].t.x + tmp.segments[1].f.x + tmp.segments[1].t.x) / 6.0;
                                        mark_point.y = (tmp.p.y * 2 - m_ydelta + tmp.segments[0].f.y + tmp.segments[0].t.y + tmp.segments[1].f.y + tmp.segments[1].t.y) / 6.0;
                                        break;
                                    case 11:
                                        mark_point.x = (tmp.p.x * 3 + m_xdelta + segment.f.x + segment.t.x) / 5.0;
                                        mark_point.y = (tmp.p.y * 3 - m_ydelta * 2 + segment.f.y + segment.t.y) / 5.0;
                                        break;
                                    case 12:
                                        mark_point.x = (tmp.p.x * 2 + segment.f.x + segment.t.x) / 4.0;
                                        mark_point.y = (tmp.p.y * 2 + segment.f.y + segment.t.y) / 4.0;
                                        break;
                                    case 13:
                                        mark_point.x = (tmp.p.x * 3 + m_xdelta + segment.f.x + segment.t.x) / 5.0;
                                        mark_point.y = (tmp.p.y * 3 - m_ydelta + segment.f.y + segment.t.y) / 5.0;
                                        break;
                                    case 14:
                                        mark_point.x = (tmp.p.x * 3 + 2 * m_xdelta + segment.f.x + segment.t.x) / 5.0;
                                        mark_point.y = (tmp.p.y * 3 - m_ydelta + segment.f.y + segment.t.y) / 5.0;
                                        break;
                                }
                                has_not_traced = true;
                            }
                        }
                }
            }
        }

        if (ring.points.size() <= m_ignore_count)
        {
            SLOG_DEBUG("提示: 忽略点数小于 {} 的闭合环.\n", m_ignore_count);
        }
        else
        {
            if (ring.points.back() != ring.points.front())  // 如果因为一些原因无法闭合,则主动使其闭合
            {
                ring.points.push_back(ring.points.front());
                // throw std::runtime_error("没有正确闭合");
            }
            // 点在面内判断
            SLOG_DEBUG("{},{}", mark_point.x, mark_point.y);
            if (point_in_ring(mark_point, ring))
            {
                ring.is_outer = 1;  // 外环
                SLOG_DEBUG("外环")
            }
            else
            {
                ring.is_outer = 0;  // 内环
                SLOG_DEBUG("内环")
            }

            all_rings.push_back(ring);
        }
    }

    for (auto &r : all_rings)
    {
        if (r.is_outer)
        {
            silly_poly tmp;
            tmp.outer_ring = r;
            r.points.clear();
            result.push_back(tmp);
        }
    }
    for (auto &poly : result)
    {
        for (auto &r : all_rings)
        {
            if (!r.is_outer && !r.points.empty())
            {
                if (point_in_ring(r.points.front(), poly.outer_ring))
                {
                    poly.inner_rings.push_back(r);
                    r.points.clear();
                }
            }
        }
    }
    for (auto &r : all_rings)
    {
        if (!r.points.empty())
        {
            SLOG_DEBUG("XXXX{}", r.points.size())
            silly_poly tmp;
            tmp.outer_ring = r;
            r.points.clear();
            result.push_back(tmp);
        }
    }
    // m_mat.clear();
    return result;
}
void silly_vectorizer::set(const std::vector<trace_square_point> &points)
{
    // 初始化矩阵, 并在周围添加框
    m_mat.clear();
    m_mat.resize(m_height + 2, std::vector<silly_trace_node>(m_width + 2));
#if defined(_OPENMP)
#pragma omp parallel for num_threads(8)
#endif
    for (int i = 0; i < points.size(); ++i)
    {
        const auto &p = points[i];
        if (p.p.x < m_left || p.p.x > m_right || p.p.y < m_bottom || p.p.y > m_top)
        {
            continue;
        }
        int c = std::round((p.p.x - m_left) / m_xdelta) + 1;
        int r = std::round((m_top - p.p.y) / m_ydelta) + 1;
        if (p.v > m_threshold_l)
        {
            m_mat[r][c].val = p.v;
            m_mat[r][c].great = 1;
        }
        else if (p.v == m_threshold_l)
        {
            m_mat[r][c].val = p.v - 0.0001;  // 避免和阈值重合
            m_mat[r][c].great = 1;
        }
        m_mat[r][c].p = p.p;
    }
    fill_mat();
    return;
}

void silly_vectorizer::mark()
{
    for (size_t r = 0; r < m_height + 1; ++r)
    {
        for (size_t c = 0; c < m_width + 1; ++c)
        {
            // 顺时针
            m_mat[r][c].cv = (m_mat[r][c].great << 3) | (m_mat[r][c + 1].great << 2) | (m_mat[r + 1][c + 1].great << 1) | m_mat[r + 1][c].great;
        }
    }
}
void silly_vectorizer::mark_edge(int r, int c, int tp)
{
}

void silly_vectorizer::find_edge()
{
    double avg = 0.;
#if defined(_OPENMP)
#pragma omp parallel for num_threads(8)
#endif
    for (int r = 0; r < m_height + 1; ++r)
    {
        for (int c = 0; c < m_width + 1; ++c)
        {
            switch (m_mat[r][c].cv)
            {
                case 1:  // 0001
                    m_mat[r][c].segments.resize(1);

                    // 2
                    CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 3
                    CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);

                    break;
                case 2:  // 0010
                    m_mat[r][c].segments.resize(1);

                    // 1
                    CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 2
                    CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 3:  // 0011
                    m_mat[r][c].segments.resize(1);

                    // 1
                    CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 3
                    CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 4:  // 0100
                    m_mat[r][c].segments.resize(1);

                    // 0
                    CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 1
                    CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 5:  // 0101	!!!
                    m_mat[r][c].segments.resize(2);

                    m_mat[r][c].segments[1].traced = 0;
                    avg = (m_mat[r][c].val + m_mat[r][c + 1].val + m_mat[r + 1][c + 1].val + m_mat[r + 1][c].val) / 4.0;

                    if (avg > m_threshold_l)
                    {
                        ///   //  0-3   1-2
                        // 0
                        CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                        // 1
                        CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[1].f);
                        // 2
                        CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[1].t);
                        // 3
                        CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    }
                    else
                    {
                        // 0  0-1 2-3
                        CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                        // 1
                        CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                        // 2
                        CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[1].f);
                        // 3
                        CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[1].t);
                    }
                    break;
                case 6:  // 0110
                    m_mat[r][c].segments.resize(1);

                    // 0
                    CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 2
                    CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 7:  // 0111
                    m_mat[r][c].segments.resize(1);

                    // 0
                    CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 3
                    CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 8:  // 1000
                    m_mat[r][c].segments.resize(1);

                    // 0
                    CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 3
                    CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 9:  // 1001
                    m_mat[r][c].segments.resize(1);

                    // 0
                    CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 2
                    CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 10:  // 1010  !!!
                    m_mat[r][c].segments.resize(2);

                    m_mat[r][c].segments[1].traced = 0;
                    avg = (m_mat[r][c].val + m_mat[r][c + 1].val + m_mat[r + 1][c + 1].val + m_mat[r + 1][c].val) / 4.0;

                    if (avg > m_threshold_l)
                    {
                        ///   //  0-1   2-3
                        // 0

                        CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                        // 1
                        CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                        // 2
                        CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[1].f);
                        // 3
                        CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[1].t);
                    }
                    else
                    {
                        // 0-3   1-2
                        CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                        // 1
                        CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[1].f);
                        // 2
                        CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[1].t);
                        // 3
                        CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    }
                    break;
                case 11:  // 1011
                    m_mat[r][c].segments.resize(1);

                    // 0
                    CALC_LINE_0_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 1
                    CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 12:  // 1100
                    m_mat[r][c].segments.resize(1);

                    // 1
                    CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 3
                    CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 13:  // 1101
                    m_mat[r][c].segments.resize(1);

                    // 1
                    CALC_LINE_1_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 2
                    CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                case 14:  // 1110
                    m_mat[r][c].segments.resize(1);

                    // 2
                    CALC_LINE_2_INTER(m_mat, r, c, m_mat[r][c].segments[0].f);
                    // 3
                    CALC_LINE_3_INTER(m_mat, r, c, m_mat[r][c].segments[0].t);
                    break;
                default:
                    m_mat[r][c].traced = 1;
            }
        }
    }
}
void silly_vectorizer::set(const std::vector<trace_square_point> &points, const double &t)
{
    // 初始化矩阵, 并在周围添加框
    m_threshold_l = t;
    set(points);
    fill_mat();
}
void silly_vectorizer::set(const std::vector<trace_square_point> &points, const double &low, const double &high)
{
    m_threshold_l = low;
    m_threshold_h = high;
    m_mat.clear();
    double fix = (m_threshold_h - m_threshold_l) * 0.001;
    m_mat.resize(m_height + 2, std::vector<silly_trace_node>(m_width + 2));
#if defined(_OPENMP)
#pragma omp parallel for num_threads(8)
#endif
    for (int i = 0; i < points.size(); ++i)
    {
        const auto &p = points[i];
        if (p.p.x < m_left || p.p.x > m_right || p.p.y < m_bottom || p.p.y > m_top)
        {
            continue;
        }
        int c = std::round((p.p.x - m_left) / m_xdelta) + 1;
        int r = std::round((m_top - p.p.y) / m_ydelta) + 1;
        if (p.v >= m_threshold_l && p.v <= m_threshold_h)
        {
            m_mat[r][c].great = 1;
            if (p.v == m_threshold_l)
            {
                m_mat[r][c].val = p.v - fix;
            }
            else if (p.v == m_threshold_h)
            {
                m_mat[r][c].val = p.v + fix;
            }
            else
            {
                m_mat[r][c].val = p.v;
            }
        }
        m_mat[r][c].p = p.p;
    }
    fill_mat();
}
std::vector<std::vector<silly_point>> silly_vectorizer::trace_all_lines()
{
    return std::vector<std::vector<silly_point>>();
}
void silly_vectorizer::fill_mat()
{
    // 填充边
    for (int i = 0; i < m_height + 2; ++i)
    {
        m_mat[i][0].p.x = m_left - m_xdelta;
        m_mat[i][0].p.y = m_top - i * m_ydelta + m_ydelta;
        m_mat[i][m_width + 1].p.x = m_right + m_ydelta;
        m_mat[i][m_width + 1].p.y = m_top - i * m_ydelta + m_ydelta;
    }

    for (int i = 0; i < m_width + 2; ++i)
    {
        m_mat[0][i].p.y = m_top + m_ydelta;
        m_mat[0][i].p.x = m_left + i * m_xdelta - m_xdelta;
        m_mat[m_height + 1][i].p.y = m_bottom - m_ydelta;
        m_mat[m_height + 1][i].p.x = m_left + i * m_xdelta - m_xdelta;
    }
}
std::vector<silly_poly> silly_vectorizer::vectorize(const std::vector<trace_square_point> &points, const double &t)
{
    set(points, t);
    mark();
    find_edge();
    return trace_all_polys();
}
void silly_vectorizer::set(const trace_grid_info &info)
{
    m_left = info.rect.min.x;
    m_right = info.rect.max.x;
    m_top = info.rect.max.y;
    m_bottom = info.rect.min.y;
    m_xdelta = info.xdelta;
    m_ydelta = info.ydelta;
    m_width = std::round((m_right - m_left) / m_xdelta);
    m_height = std::round((m_top - m_bottom) / m_ydelta);
}
void silly_vectorizer::set(const trace_algo_info &info)
{
    m_fill = info.fill;
    m_ignore_count = info.ignore_count;
    m_interpolation_mode = info.interpolation_mode;
    m_smooth = info.smooth;
    m_threshold_l = info.threshold;
}
silly_vectorizer::silly_vectorizer(const silly_vectorizer &right)
{
    m_height = right.m_height;
    m_width = right.m_width;
    m_left = right.m_left;
    m_right = right.m_right;
    m_top = right.m_top;
    m_bottom = right.m_bottom;
    m_xdelta = right.m_xdelta;
    m_ydelta = right.m_ydelta;
    m_fill = right.m_fill;
    m_ignore_count = right.m_ignore_count;
    m_interpolation_mode = right.m_interpolation_mode;
    m_smooth = right.m_smooth;
    m_threshold_l = right.m_threshold_l;
    m_threshold_h = right.m_threshold_h;
}
silly_vectorizer &silly_vectorizer::operator=(const silly_vectorizer &right)
{
    m_height = right.m_height;
    m_width = right.m_width;
    m_left = right.m_left;
    m_right = right.m_right;
    m_top = right.m_top;
    m_bottom = right.m_bottom;
    m_xdelta = right.m_xdelta;
    m_ydelta = right.m_ydelta;
    m_fill = right.m_fill;
    m_ignore_count = right.m_ignore_count;
    m_interpolation_mode = right.m_interpolation_mode;
    m_smooth = right.m_smooth;
    m_threshold_l = right.m_threshold_l;
    m_threshold_h = right.m_threshold_h;
    return *this;
}

static double bezier_In(const double &t, double x1, double x2, double x3)
{
    // 参数校验：确保 t 在 0 到 1 之间
    if (t < 0.0 || t > 1.0)
    {
        throw std::invalid_argument("Parameter t must be in the range [0, 1]");
    }

    double controlPoint1 = (x2 - x1) * t + x1;                                // 第一个控制点
    double controlPoint2 = (x3 - x2) * t + x2;                                // 第二个控制点
    double finalPoint = (controlPoint2 - controlPoint1) * t + controlPoint1;  // 最终点

    return finalPoint;
}
#include <math/spline/silly_b_spline.h>
#include <math/silly_bezier_curve.h>
std::vector<silly_poly> silly_vectorizer::smooth_poly(const std::vector<silly_poly> &polys)

{
    std::vector<silly_poly> smooth_polys;
    for (auto poly : polys)
    {
        silly_poly tmp;
        /*tmp.outer_ring.points = silly_bezier_curve::bezier_smooth(poly.outer_ring.points, poly.outer_ring.points.size() * 5); //smooth_ring(poly.outer_ring);

        for (auto ring : poly.inner_rings)
        {
            // tmp.inner_rings.push_back(smooth_ring(ring));

            tmp.inner_rings.push_back({silly_b_spline::cubic_interpolation(ring.points, ring.points.size() * 5)});
        }*/
        smooth_polys.push_back(tmp);
    }
    return smooth_polys;
}

bool is_less_than_slope(const silly_point &p1, const silly_point &p2, const silly_point &p3, const double angle)
{
    double x1_diff = p2.x - p1.x;
    double y1_diff = p2.y - p1.y;
    double x2_diff = p3.x - p1.x;
    double y2_diff = p3.y - p1.y;
    double angle1 = 90.0f, angle2 = 90.0f;
    // 检查分母是否接近零，避免除以零的情况
    if (std::abs(x1_diff) > SU_EPSILON)
    {
        angle1 = std::atan2(y1_diff, x1_diff) * 180.0 / PI;
    }
    if (std::abs(x2_diff) > SU_EPSILON)
    {
        angle2 = std::atan2(y2_diff, x2_diff) * 180.0 / PI;
    }

    // 考虑角度的周期性，确保不丢失解决方案
    if (std::abs(angle1 - angle2) < angle)
    {
        return true;
    }

    return false;
}

std::vector<silly_poly> silly_vectorizer::simplify_poly_less_angle(const std::vector<silly_poly> &polys, const double &angle)
{
    std::vector<silly_poly> simple_polys;
    for (auto poly : polys)
    {
        silly_poly simple_poly;

        simple_poly.outer_ring = simplify_ring_douglas(poly.outer_ring, angle);

        for (auto ring : poly.inner_rings)
        {
            simple_poly.inner_rings.push_back(simplify_ring_douglas(ring, angle));
        }

        simple_polys.emplace_back(simple_poly);
        // break;
    }

    return simple_polys;
}
std::vector<silly_poly> silly_vectorizer::simplify_poly_mid_point(const std::vector<silly_poly> &polys)
{
    std::vector<silly_poly> simple_polys;
    for (auto poly : polys)
    {
        silly_poly simple_poly;

        size_t p_size = poly.outer_ring.points.size();
        double x = 0, y = 0;
        int j = 0;
        for (; j < poly.outer_ring.points.size() - 1; j++)
        {
            x = (poly.outer_ring.points[j].x + poly.outer_ring.points[j + 1].x) / 2;
            y = (poly.outer_ring.points[j].y + poly.outer_ring.points[j + 1].y) / 2;
            simple_poly.outer_ring.points.push_back({x, y});
        }
        x = (poly.outer_ring.points[j].x + poly.outer_ring.points[0].x) / 2;
        y = (poly.outer_ring.points[j].y + poly.outer_ring.points[0].y) / 2;
        simple_poly.outer_ring.points.push_back({x, y});
        simple_poly.outer_ring.points.push_back(poly.outer_ring.points[0]);

        for (auto ring : poly.inner_rings)
        {
            while (ring.points.back() == ring.points.front())
            {
                ring.points.pop_back();
            }
            silly_ring simple_ring;
            simple_ring.points.push_back(ring.points[0]);
            p_size = ring.points.size();
            for (j = 0; j < ring.points.size() - 1; j++)
            {
                // 取中点作为控制点
                x = (ring.points[j].x + ring.points[j + 1].x) / 2;
                y = (ring.points[j].y + ring.points[j + 1].y) / 2;
                simple_ring.points.push_back({x, y});
            }
            x = (ring.points[j].x + ring.points[0].x) / 2;
            y = (ring.points[j].y + ring.points[0].y) / 2;
            simple_ring.points.push_back({x, y});
            simple_ring.points.push_back(simple_ring.points[0]);
            simple_poly.inner_rings.emplace_back(simple_ring);
        }

        simple_polys.emplace_back(simple_poly);
    }

    return simple_polys;
}
std::vector<silly_poly> silly_vectorizer::simplify_poly_same_slope(const std::vector<silly_poly> &polys)
{
    std::vector<silly_poly> result;
    for (auto poly : polys)
    {
        silly_poly tmp;
        tmp.outer_ring = simplify_ring_same_slope(poly.outer_ring);
        for (auto ring : poly.inner_rings)
        {
            tmp.inner_rings.push_back(simplify_ring_same_slope(ring));
        }
        result.push_back(tmp);
        // break;
    }

    return result;
}
size_t silly_vectorizer::width() const
{
    return m_width;
}
size_t silly_vectorizer::height() const
{
    return m_height;
}
silly_ring silly_vectorizer::simplify_ring_same_slope(const silly_ring &ring)
{
    silly_ring result;
    if (ring.points.size() < 3)
    {
        return ring;
    }
    int i = 0;
    int num = ring.points.size();
    result.points.push_back(ring.points[0]);
    for (; i < num; ++i)
    {
        int n1 = (i + 1) % num, n2 = (i + 2) % num;
        double dx1 = ring.points[n1].x - ring.points[i].x;
        double dy1 = ring.points[n1].y - ring.points[i].y;
        double dx2 = ring.points[n2].x - ring.points[n1].x;
        double dy2 = ring.points[n2].y - ring.points[n1].y;

        double slope1 = 1.0e10, slope2 = 1.0e10;
        if (std::abs(dx1) > SU_EPSILON)
        {
            slope1 = dy1 / dx1;
        }
        if (std::abs(dx2) > SU_EPSILON)
        {
            slope2 = dy2 / dx2;
        }

        if (std::abs(slope1 - slope2) < SU_EPSILON)
        {
            continue;
        }
        result.points.push_back(ring.points[i]);
    }

    return result;
}
silly_ring silly_vectorizer::simplify_ring_less_angle(const silly_ring &ring, double angle)
{
    silly_ring result;
    if (ring.points.size() < 3)
    {
        return result;
    }

    int num = ring.points.size();
    int limit_num = num;
    if (ring.points.front() == ring.points.back())
    {
        limit_num--;
    }
    result.points.push_back(ring.points[0]);
    for (int i = 1; i < num; ++i)
    {
        int n1 = (i - 1) % num, n2 = (i + 1) % num;
        if (!is_less_than_slope(ring.points[i], ring.points[n1], ring.points[n2], angle))
        {
            result.points.push_back(ring.points[i]);
        }
    }
    if (result.points.front() != result.points.back())
    {
        result.points.push_back(result.points.front());
    }
    return result;
}
silly_ring silly_vectorizer::smooth_ring(const silly_ring &ring)
{
    silly_ring result;

    double x = 0, y = 0;
    size_t p_size = ring.points.size();

    for (int j = 0; j < p_size; j++)
    {
        double bzr_step = 1.0 / m_smooth;
        double t = bzr_step;

        // 取中点作为控制点
        size_t m1 = j + 1, m2 = j + 2;
        m1 = m1 < p_size ? m1 : m1 - p_size;
        m2 = m2 < p_size ? m2 : m2 - p_size;
        double x0 = (ring.points[j].x + ring.points[m1].x) / 2;
        double y0 = (ring.points[j].y + ring.points[m1].y) / 2;
        double x2 = (ring.points[m2].x + ring.points[m1].x) / 2;
        double y2 = (ring.points[m2].y + ring.points[m1].y) / 2;

        result.points.emplace_back(x0, y0);
        for (int k = 0; k < m_smooth; k++)
        {
            x = bezier_In(t, x0, ring.points[m1].x, x2);
            y = bezier_In(t, y0, ring.points[m1].y, y2);
            result.points.emplace_back(x, y);
            t += bzr_step;
        }
    }
    result.points.push_back(ring.points[0]);
    return result;
}
silly_ring silly_vectorizer::simplify_ring_less_angle_1(const silly_ring &ring, double angle)
{
    silly_ring result;
    if (ring.points.size() < 3)
    {
        return result;
    }

    int num = ring.points.size();
    int limit_num = num;
    if (ring.points.front() == ring.points.back())
    {
        limit_num--;
    }
    result.points.push_back(ring.points[0]);
    int c = 0;
    for (int i = 0; i < num; ++i)
    {
        int n1 = (i + 1) % num, n2 = (i + 2) % num;
        if (!is_less_than_slope(ring.points[c], ring.points[n1], ring.points[n2], angle))
        {
            result.points.push_back(ring.points[i]);
            c = i;
        }
    }
    if (result.points.front() != result.points.back())
    {
        result.points.push_back(result.points.front());
    }
    return result;
}
silly_ring silly_vectorizer::simplify_ring_douglas(const silly_ring &ring, double dist)
{
    silly_ring result;
    if (ring.points.size() < 3)
    {
        return result;
    }
    size_t p_size = ring.points.size();
    std::vector<double> inputs;
    for (auto p : ring.points)
    {
        inputs.emplace_back(p.x);
        inputs.emplace_back(p.y);
    }
    std::vector<double> outputs;
    silly_vacuate::douglas_peucker(dist, inputs, outputs);
    for (auto i = 0; i < outputs.size() / 2; i++)
    {
        result.points.push_back({outputs[i * 2], outputs[i * 2 + 1]});
    }

    return result;
}
