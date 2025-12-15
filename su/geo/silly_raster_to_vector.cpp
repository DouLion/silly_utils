//
// Created by dell on 2023/12/6.
//

#include "silly_raster_to_vector.h"
#include <log/silly_log.h>
#include <geo/vacuate/silly_vacuate.h>
#include <pool/silly_thread_pool.h>
#include <datetime/silly_timer.h>

#define EPS_00 0.00000000001
static bool PointEQ(const suPoint& p1, const suPoint& p2)
{
    return std::abs(p1.x - p2.x) < EPS_00 && std::abs(p1.y - p2.y) < EPS_00;
}

MatchingSquares::~MatchingSquares()
{
    m_mat0.release();
    for (auto& [_, m] : m_l2wb)
    {
        m.release();
    }
}
void MatchingSquares::SetLevels(const std::map<int, float>& levels)
{
    m_l2th = levels;
}
void MatchingSquares::SetMat(const suMatrix<float>& mat)
{
    if (mat.row() < 4 || mat.col() < 4)
    {
        SLOG_ERROR("????这么小的矩阵,自己掰指头去????")
        return;
    }
    m_height = mat.row() + 2;
    m_width = mat.col() + 2;
    m_mat0.create(m_height, m_width, true);
    for (int r = 1; r < m_height - 1; ++r)
    {
        std::memcpy((m_mat0[r] + 1), mat[r - 1], sizeof(float) * mat.col());
    }
}

void MatchingSquares::UseThreadPool()
{
    m_useThreadPool = true;
}

void MatchingSquares::Calc()
{
    if (!m_mat0.valid() || m_l2th.empty())
    {
        return;
    }
    suTimer _timer;
    if (m_useThreadPool)
    {
        suThreadPool pool;
        for (auto& [l, _] : m_l2th)
        {
            pool.enqueue(&MatchingSquares::MakeWhiteBlack, this, l);
        }
    }
    else
    {
        for (auto& [l, _] : m_l2th)
        {
            MakeWhiteBlack(l);
        }
    }
    SLOG_DEBUG("构建黑白格点: {}ms", _timer.elapsed_ms());
    _timer.restart();
    if (m_useThreadPool)
    {
        suThreadPool pool;
        for (auto& [l, _] : m_l2th)
        {
            pool.enqueue(&MatchingSquares::TraceWhiteBlackSegment, this, l);
        }
    }
    else
    {
        for (auto& [l, _] : m_l2th)
        {
            TraceWhiteBlackSegment(l);
        }
    }
    SLOG_DEBUG("构建格点线段: {}ms", _timer.elapsed_ms());
    if (m_useThreadPool)
    {
        suThreadPool pool;
        for (auto& [l, _] : m_l2th)
        {
            pool.enqueue(&MatchingSquares::TraceContinuousPoints, this, l);
        }
    }
    else
    {
        for (auto& [l, _] : m_l2th)
        {
            TraceContinuousPoints(l);
        }
    }
    SLOG_DEBUG("连接格点线段: {}ms", _timer.elapsed_ms());
}
std::vector<suLine> MatchingSquares::GetLines(const ReadParam& param) const
{
    std::vector<suLine> ret;
    if (!MAP_HAS(m_l2points, param.level))
    {
        return ret;
    }
    for (const auto& l : m_l2points.at(param.level))
    {
        suLine nl;
        for (const auto& p : l)
        {
            suPoint np;
            np.x = param.bound.min.x + p.x * param.dx - param.dx;
            np.y = param.bound.max.y - p.y * param.dy + param.dy;
            nl.push_back(np);
        }
        ret.push_back(nl);
    }
    return ret;
}

std::vector<suPoly> MatchingSquares::GetPolys(const ReadParam& param) const
{
    std::vector<suPoly> ret;
    if (!MAP_HAS(m_l2points, param.level))
    {
        return ret;
    }
    std::vector<suRing> rings;
    const std::vector<std::list<suPoint>>& lines = m_l2points.at(param.level);
    const float t = m_l2th.at(param.level);
    for (const auto& l : lines)
    {
        suRing nr;

        for (const auto& p : l)
        {
            suPoint np;
            np.x = param.bound.min.x + p.x * param.dx - param.dx;
            np.y = param.bound.max.y - p.y * param.dy + param.dy;
            nr.points.push_back(np);
        }
        if (param.ignore > 0)
        {
            if (nr.area()/param.dx/ param.dy > param.ignore)
            {
                continue;
            }
        }
        {
            size_t r = l.front().y;
            size_t c = l.front().x;
            bool greater = (m_mat0.at(r, c) >= t);
            suPoint ckp;
            ckp.x = param.bound.min.x + c * param.dx - param.dx;
            ckp.y = param.bound.max.y - r * param.dy + param.dy;
            if (greater == SU_POINT_IN_CLOSED_RING(ckp, nr.points))
            {
                nr.is_outer = 1;
                suPoly tp;
                tp.outer = nr;
                ret.push_back(tp);
            }
            else
            {
                nr.is_outer = 0;
                rings.push_back(nr);
            }
        }
    }
    //
    for (auto& poly : ret)
    {
        for (auto& r : rings)
        {
            if (r.is_outer)
            {
                continue;
            }
            if (SU_POINT_IN_CLOSED_RING(r.points.front(), poly.outer.points))
            {
                r.is_outer = 1;
                poly.holes.push_back(r);
            }
        }
    }

    return ret;
    return {};
}

void MatchingSquares::MakeWhiteBlack(const int& level)
{
    suMatrix<int8_t> wb;
    wb.create(m_height, m_width);
    float t = m_l2th[level];
    for (size_t r = 0; r < m_height; r++)
    {
        for (size_t c = 0; c < m_width; c++)
        {
            float& v = m_mat0.at(r, c);
            //  当与阈值相等时,略微提高此值
            //  避免后续插值及追踪时出现意外情况
            if (v == t)
            {
                v += 0.0001;
            }
            if (v < t)
            {
                wb.at(r, c) = 1;
            }
        }
    }
    m_l2wb[level] = wb;
}

void MatchingSquares::TraceWhiteBlackSegment(const int& level)
{
    auto& wb = m_l2wb[level];
    float t = m_l2th[level];
    // 先分类
    bool interp = m_mode == eMathingMode::Interp;
    int markNum = 0;
    for (size_t r = 0; r < m_height - 1; r++)
    {
        for (size_t c = 0; c < m_width - 1; c++)
        {
            int8_t& v = wb[r][c];
            wb[r][c] = (wb[r][c] << 3) | (wb[r][c + 1] << 2) | (wb[r + 1][c + 1] << 1) | wb[r + 1][c];
            markNum++;
        }
    }
    std::cout << markNum << std::endl;
    // 超过16表示已经标记过
    int num10 = 0, num5 = 0;
    std::vector<suSegment> lines;

    for (size_t r = 0; r < m_height - 1; r++)
    {
        for (size_t c = 0; c < m_width - 1; c++)
        {
            suSegment s, s2;
            float avg = 0;
            int8_t& v = wb[r][c];
            switch (v)
            {
                case 1:  // 0001
                    // 2
                    s.p0 = X_POINT_ON_EDGE_2(r, c, t, interp);
                    // 3
                    s.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    break;
                case 2:  // 0010
                    // 1
                    s.p0 = X_POINT_ON_EDGE_1(r, c, t, interp);
                    // 2
                    s.p1 = X_POINT_ON_EDGE_2(r, c, t, interp);
                    break;
                case 3:  // 0011
                    // 1
                    s.p0 = X_POINT_ON_EDGE_1(r, c, t, interp);
                    // 3
                    s.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    break;
                case 4:  // 0100
                    // 0
                    s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                    // 1
                    s.p1 = X_POINT_ON_EDGE_1(r, c, t, interp);
                    break;
                case 5:  // 0101	!!!
                    avg = (m_mat0[r][c] + m_mat0[r][c + 1] + m_mat0[r + 1][c + 1] + m_mat0[r + 1][c]) / 4.0;
                    num5++;
                    if (avg > t)
                    {
                        ///   //  0-3   1-2
                        // 0
                        s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                        // 1
                        s.p1 = X_POINT_ON_EDGE_1(r, c, t, interp);
                        // 2
                        s2.p0 = X_POINT_ON_EDGE_2(r, c, t, interp);
                        // 3
                        s2.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    }
                    else
                    {
                        // 0  0-1 2-3
                        s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                        // 1
                        s.p1 = X_POINT_ON_EDGE_1(r, c, t, interp);
                        // 2
                        s2.p0 = X_POINT_ON_EDGE_2(r, c, t, interp);
                        // 3
                        s2.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    }
                    break;
                case 6:  // 0110
                    // 0
                    s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                    // 2
                    s.p1 = X_POINT_ON_EDGE_2(r, c, t, interp);
                    break;
                case 7:  // 0111
                    // 0
                    s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                    // 3
                    s.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    break;
                case 8:  // 1000
                         // 0
                    s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                    // 3
                    s.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    break;
                case 9:  // 1001

                    // 0
                    s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                    // 2
                    s.p1 = X_POINT_ON_EDGE_2(r, c, t, interp);
                    break;
                case 10:  // 1010  !!!
                    avg = (m_mat0[r][c] + m_mat0[r][c + 1] + m_mat0[r + 1][c + 1] + m_mat0[r + 1][c]) / 4.0;
                    num10++;
                    if (avg > t)
                    {
                        ///   //  0-1   2-3
                        // 0

                        s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                        // 1
                        s.p1 = X_POINT_ON_EDGE_1(r, c, t, interp);
                        // 2
                        s2.p0 = X_POINT_ON_EDGE_2(r, c, t, interp);
                        // 3
                        s2.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    }
                    else
                    {
                        // 0-3   1-2
                        s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                        // 1
                        s.p1 = X_POINT_ON_EDGE_1(r, c, t, interp);
                        // 2
                        s2.p0 = X_POINT_ON_EDGE_2(r, c, t, interp);
                        // 3
                        s2.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    }
                    break;
                case 11:  // 1011
                          // 0
                    s.p0 = X_POINT_ON_EDGE_0(r, c, t, interp);
                    // 1
                    s.p1 = X_POINT_ON_EDGE_1(r, c, t, interp);
                    break;
                case 12:  // 1100
                          // 1
                    s.p0 = X_POINT_ON_EDGE_1(r, c, t, interp);
                    // 3
                    s.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    break;
                case 13:  // 1101
                          // 1
                    s.p0 = X_POINT_ON_EDGE_1(r, c, t, interp);
                    // 2
                    s.p1 = X_POINT_ON_EDGE_2(r, c, t, interp);
                    break;
                case 14:  // 1110

                    // 2
                    s.p0 = X_POINT_ON_EDGE_2(r, c, t, interp);
                    // 3
                    s.p1 = X_POINT_ON_EDGE_3(r, c, t, interp);
                    break;
                default:
                    // m_mat0[r][c].traced = 1;
                    break;
            }
            if (s.p0.x > 0)
            {
                lines.push_back(s);
            }

            if (s2.p0.x > 0)
            {
                lines.push_back(s2);
            }
        }
    }
    SLOG_DEBUG("Case 10: {}, Case 5 : {}", num10, num5);
    if (lines.empty())
    {
        SLOG_WARN("未追踪到")
    }
    SLOG_INFO("线段数量: {}", lines.size())
    m_l2seg[level] = lines;
}

static suPoint* MatchPoint(const suSegment& s, const suPoint& p)
{
    if (PointEQ(s.p0, p))
    {
        return const_cast<suPoint*>(&s.p1);
    }
    if (PointEQ(s.p1, p))
    {
        return const_cast<suPoint*>(&s.p0);
    }
    return nullptr;
}
void MatchingSquares::TraceContinuousPoints(const int& level)
{
    auto& segments = m_l2seg[level];

    std::vector<std::list<suPoint>> lineParts;
    std::vector<int8_t> mark(segments.size(), 0);

    while (true)
    {
        // 1 先驱任意一个未被标记的点
        std::list<suPoint> tmpLine;
        for (int i = 0; i < mark.size(); ++i)
        {
            if (!mark[i])
            {
                tmpLine.push_back(segments[i].p0);
                tmpLine.push_back(segments[i].p1);
                mark[i] = 1;
                break;
            }
        }
        // 所有的点都被标记的话,则全部完成
        if (tmpLine.empty())
        {
            break;
        }
        size_t lastSize = tmpLine.size();
        // 当首尾不相交时, 遍历所有的点
        while (!PointEQ(tmpLine.front(), tmpLine.back()))
        {
            for (int i = 0; i < segments.size(); ++i)
            {
                if (mark[i])
                {
                    continue;
                }
                auto& p0 = tmpLine.front();
                auto& p1 = tmpLine.back();

                suPoint* np = MatchPoint(segments[i], p0);
                if (np)
                {  // 与前端的点能匹配上, 就在前面添加一个点
                    tmpLine.push_front(*np);
                    mark[i] = 1;
                    continue;
                }
                np = MatchPoint(segments[i], p1);
                if (np)
                {  // 与末尾的点能匹配上, 就在最后添加一个点
                    tmpLine.push_back(*np);
                    mark[i] = 1;
                }
            }
            // 再也找不到新点, 就跳过
            if (tmpLine.size() == lastSize)
            {
                break;
            }
            lastSize = tmpLine.size();
        }

        lineParts.push_back(tmpLine);
    }
    m_l2points[level] = lineParts;
    m_l2wb[level].release();
    m_l2seg[level].clear();
    SLOG_DEBUG("层级 {} 共  {} 条线", level, lineParts.size())
}

suPoint MatchingSquares::X_POINT_ON_EDGE_0(const int& r, const int& c, const float& t, const bool& interp) const
{
    suPoint p;
    float l = m_mat0.at(r, c);
    float h = m_mat0.at(r, c + 1);
    p.y = r;
    if (interp)
    {
        p.x = c + (t - l) / (h - l);
    }
    else
    {
        p.x = c + 0.5;
    }
    return p;
}

suPoint MatchingSquares::X_POINT_ON_EDGE_1(const int& r, const int& c, const float& t, const bool& interp) const
{
    suPoint p;
    float l = m_mat0.at(r, c + 1);
    float h = m_mat0.at(r + 1, c + 1);
    p.x = c + 1;
    if (interp)
    {
        p.y = r + (t - l) / (h - l);
    }
    else
    {
        p.y = r + 0.5;
    }
    return p;
}

suPoint MatchingSquares::X_POINT_ON_EDGE_2(const int& r, const int& c, const float& t, const bool& interp) const
{
    suPoint p;
    float l = m_mat0.at(r + 1, c);
    float h = m_mat0.at(r + 1, c + 1);
    p.y = r + 1;
    if (interp)
    {
        p.x = c + (t - l) / (h - l);
    }
    else
    {
        p.x = c + 0.5;
    }
    return p;
}

suPoint MatchingSquares::X_POINT_ON_EDGE_3(const int& r, const int& c, const float& t, const bool& interp) const
{
    suPoint p;
    float l = m_mat0.at(r, c);
    float h = m_mat0.at(r + 1, c);
    p.x = c;
    if (interp)
    {
        p.y = r + (t - l) / (h - l);
    }
    else
    {
        p.y = r + 0.5;
    }
    return p;
}
