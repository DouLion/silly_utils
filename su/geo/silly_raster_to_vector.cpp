//
// Created by dell on 2023/12/6.
//

#include "silly_raster_to_vector.h"
#include <log/silly_log.h>

// #define CASE_01_NEXT(r, c, )

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
            if (n1.p.lgtd == n2.p.lgtd)
            {
                point.lgtd = n1.p.lgtd;

                point.lttd = n1.p.lttd + m_ydelta * (m_theshold - n1.val) / (n2.val - n1.val);
            }
            else if (n1.p.lttd == n2.p.lttd)
            {
                point.lttd = n1.p.lttd;
                point.lgtd = n1.p.lgtd + m_xdelta * (m_theshold - n1.val) / (n2.val - n1.val);
            }
            else
            {
                SLOG_ERROR("\n{:02f}, {:02f}\n{:02f}, {:02f}", n1.p.lgtd, n1.p.lttd, n2.p.lgtd, n2.p.lttd)
                throw std::runtime_error("经纬度必须相等");
            }
#else
            if (n1.p.lgtd == n2.p.lgtd)
            {
                point.lgtd = n1.p.lgtd;

                point.lttd = n1.p.lttd + m_ydelta * (m_theshold - n1.val) / (n2.val - n1.val);
            }
            else
            {
                point.lttd = n1.p.lttd;
                point.lgtd = n1.p.lgtd + m_xdelta * (m_theshold - n1.val) / (n2.val - n1.val);
            }
#endif
            break;
        case 2:  // 直接取中点
            point.lgtd = (n1.p.lgtd + n2.p.lgtd) / 2.;
            point.lttd = (n1.p.lttd + n2.p.lttd) / 2.;
        default:
            break;
    }
}

void silly_vectorizer::trace_one_line(int r0l, int c0l, silly_ring &ring)
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
}

bool silly_vectorizer::point_in_ring(const silly_point &point, const silly_ring &ring)
{
    size_t i, j;
    double d;
    int c = 0;
    size_t num = ring.points.size();
    for (i = 0, j = num - 1; i < num; j = i++)
    {
        d = (ring.points[j].lgtd - ring.points[i].lgtd) * (point.lttd - ring.points[i].lttd) / (ring.points[j].lttd - ring.points[i].lttd) + ring.points[i].lgtd;
        if (point.lgtd == d)
        {
            return 0;
        }

        if ((((ring.points[i].lttd <= point.lttd) && (point.lttd < ring.points[j].lttd) || ((ring.points[j].lttd <= point.lttd) && (point.lttd < ring.points[i].lttd))) && (point.lgtd < d)))
        {
            c = !c;
        }
    }
    return c;
}

std::vector<silly_poly> silly_vectorizer::trace_all_rings()
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
                switch (m_mat[r][c].cv)
                {
                    case 0:
                    case 15:
                        break;
                    default:
                        for (auto &segment : m_mat[r][c].segments)
                        {
                            if (!segment.traced)
                            {
                                ring.points.push_back(segment.f);
                                ring.points.push_back(segment.t);
                                segment.traced = 1;

                                // 递归查找下一个点
                                trace_one_line(r, c, ring);
                                switch (m_mat[r][c].cv)
                                {
                                    case 8:
                                    case 9:
                                    case 10:
                                    case 11:
                                    case 12:
                                    case 13:
                                    case 14:
                                        // 取当前点就行
                                        mark_point = m_mat[r][c].p;
                                        break;

                                    case 5:
                                        if (m_mat[r][c].val > m_theshold)
                                        {
                                            mark_point = m_mat[r][c].p;
                                        }
                                        else
                                        {
                                            mark_point = m_mat[r][c + 1].p;
                                        }
                                        break;
                                    case 7:
                                    case 4:
                                    case 6:
                                        mark_point = m_mat[r][c + 1].p;
                                        // 右上
                                        break;
                                    case 1:
                                    case 3:
                                        mark_point = m_mat[r + 1][c].p;
                                        // 左下
                                        break;
                                    case 2:
                                        mark_point = m_mat[r + 1][c + 1].p;
                                        // 右下
                                        break;
                                }
                                has_not_traced = true;
                            }
                        }
                }
            }
        }

        if (ring.points.size() < m_ignore_count)
        {
            printf("Hint: Ignore poly that point count less than %d.\n", m_ignore_count);
        }
        else
        {
            if (ring.points.back() != ring.points.front())  // 如果因为一些原因无法闭合,则主动使其闭合
            {
                throw std::runtime_error("没有正确闭合");
            }
            // 点在面内判断
            if (!point_in_ring(mark_point, ring))
            {
                ring.is_outer = 0;  // 内环
            }
            else
            {
                ring.is_outer = 1;  // 外环
            }

            // std::cout << "Point num: " << ring.points.size() << ", is inner: " << ring.is_outer<< std::endl;

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
           /* SLOG_DEBUG("XXXX")
            silly_poly tmp;
            tmp.outer_ring = r;
            r.points.clear();*/
           result[0].inner_rings.push_back(r);
        }
    }
    return result;
}
void silly_vectorizer::set(const std::vector<trace_square_point> &points)
{
    int cnt = 0;
    // 初始化矩阵, 并在周围添加框
    m_mat.resize(m_height + 2, std::vector<silly_trace_node>(m_width + 2));
    // #pragma omp parallel for num_threads(8)
    for (int i = 0; i < points.size(); ++i)
    {
        const auto &p = points[i];
        if (p.p.lgtd < m_left || p.p.lgtd > m_right || p.p.lttd < m_bottom || p.p.lttd > m_top)
        {
            continue;
        }
        int c = std::round((p.p.lgtd - m_left) / m_xdelta) + 1;
        int r = std::round((m_top - p.p.lttd) / m_ydelta) + 1;
        if (p.v > m_theshold)
        {
            m_mat[r][c].val = p.v;
            m_mat[r][c].great = 1;
            cnt++;
        }
        else if (p.v == m_theshold)
        {
            m_mat[r][c].val = p.v - 0.0001;  // 避免和阈值重合
            m_mat[r][c].great = 1;
            cnt++;
        }
        m_mat[r][c].p = p.p;
    }
    // 初始化边
    for (int i = 0; i < m_height + 2; ++i)
    {
        m_mat[i][0].p.lgtd = m_left - m_xdelta;
        m_mat[i][0].p.lttd = m_top - i * m_ydelta + m_ydelta;
        m_mat[i][m_width + 1].p.lgtd = m_right + m_ydelta;
        m_mat[i][m_width + 1].p.lttd = m_top - i * m_ydelta + m_ydelta;
    }

    for (int i = 0; i < m_width + 2; ++i)
    {
        m_mat[0][i].p.lttd = m_top + m_ydelta;
        m_mat[0][i].p.lgtd = m_left + i * m_xdelta - m_xdelta;
        m_mat[m_height + 1][i].p.lttd = m_bottom - m_ydelta;
        m_mat[m_height + 1][i].p.lgtd = m_left + i * m_xdelta - m_xdelta;
    }

    /*  for (int r = 0; r < m_height + 1; ++r)
      {
          for (int c = 0; c < m_width + 1; ++c)
          {
              if (m_mat[r][c].great)
              {
                  SLOG_DEBUG("\n<===R {:03d}=====C {:03d}=====>\n{:02f}, {:02f}\n{:02f}, {:02f}\n{:02f}, {:02f}",r, c, m_mat[r][c].p.lgtd, m_mat[r][c].p.lttd, m_mat[r][c + 1].p.lgtd, m_mat[r][c + 1].p.lttd, m_mat[r+1][c].p.lgtd, m_mat[r+1][c].p.lttd)

                  assert(m_mat[r][c].p.lttd == m_mat[r][c + 1].p.lttd);
                  assert((m_mat[r][c].p.lgtd + m_xdelta) == m_mat[r][c + 1].p.lgtd);
                  assert((m_mat[r][c].p.lttd - m_ydelta) == m_mat[r+1][c].p.lttd);
                  assert(m_mat[r][c].p.lgtd == m_mat[r+1][c].p.lgtd);

              }
          }
      }*/

    std::cout << cnt << std::endl;
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

void silly_vectorizer::find_edge()
{
    double avg = 0.;
    // #pragma omp parallel for num_threads(8)
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

                    if (avg > m_theshold)
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

                    if (avg > m_theshold)
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
