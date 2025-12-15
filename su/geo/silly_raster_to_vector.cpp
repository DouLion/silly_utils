//
// Created by dell on 2023/12/6.
//

#include "silly_raster_to_vector.h"
#include <log/silly_log.h>
#include <geo/vacuate/silly_vacuate.h>
#include <pool/silly_thread_pool.h>
#ifndef NDEBUG
static int SegNum = 0;
#endif
static bool PointEQ(const suPoint& p1, const suPoint& p2)
{
    return std::abs(p1.x - p2.x) < 0.01 && std::abs(p1.y - p2.y) < 0.01;
}

static bool SegmentEQ(const suSegment& s1, const suSegment& s2)
{
    if (PointEQ(s1.p0, s2.p0))
    {
        return PointEQ(s1.p1, s2.p1);
    }
    if (PointEQ(s1.p0, s2.p1))
    {
        return PointEQ(s1.p1, s2.p0);
    }
    return false;
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
    m_levels = levels;
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
bool MatchingSquares::TracePoly()
{
    if (!m_mat0.valid() || m_levels.empty())
    {
        return false;
    }
    MakeWhiteBlack();
    TraceWhiteBlack(m_levels.begin()->first);
    return true;
}
bool MatchingSquares::TraceLine()
{
    if (!m_mat0.valid() || m_levels.empty())
    {
        return false;
    }
    MakeWhiteBlack();
    TraceWhiteBlack(m_levels.begin()->first);
    return true;
    return false;
}

std::vector<suLine> MatchingSquares::GetLines(const ReadParam& param)
{
    std::vector<suLine> ret = TraceLine(m_levels.begin()->first);
    for (auto& l : ret)
    {
        for (auto& p : l)
        {
            p.x = param.bound.min.x + p.x * param.dx - param.dx;
            p.y = param.bound.max.y - p.y * param.dy + param.dy;
        }
    }
    return ret;
}

std::vector<suPoly> MatchingSquares::GetPolys(const ReadParam& param)
{
    // TraceRing(m_levels.begin()->first);
    return {};
}
void MatchingSquares::MakeWhiteBlack()
{
    // suThreadPool pool(m_levels.size());
    for (auto& [l, _] : m_levels)
    {
        MakeWhiteBlack(l);
    }
}

void MatchingSquares::MakeWhiteBlack(const int& level)
{
    suMatrix<int8_t> wb;
    wb.create(m_height, m_width);
    float t = m_levels[level];
    for (size_t r = 0; r < m_height; r++)
    {
        for (size_t c = 0; c < m_width; c++)
        {
            if (m_mat0.at(r, c) < t)
            {
                wb.at(r, c) = 1;
            }
        }
    }
    m_l2wb[level] = wb;
}

void MatchingSquares::TraceWhiteBlack(const int& level)
{
    auto& wb = m_l2wb[level];
    float t = m_levels[level];
    // 先分类
    bool useHalf = false;
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
                    s.p0 = CALC_LINE_2_INTER(r, c, t, useHalf);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    break;
                case 2:  // 0010
                    // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, t, useHalf);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, t, useHalf);
                    break;
                case 3:  // 0011
                    // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, t, useHalf);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    break;
                case 4:  // 0100
                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                    // 1
                    s.p1 = CALC_LINE_1_INTER(r, c, t, useHalf);
                    break;
                case 5:  // 0101	!!!
                    avg = (m_mat0[r][c] + m_mat0[r][c + 1] + m_mat0[r + 1][c + 1] + m_mat0[r + 1][c]) / 4.0;
                    num5++;
                    if (avg > t)
                    {
                        ///   //  0-3   1-2
                        // 0
                        s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, t, useHalf);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, t, useHalf);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    }
                    else
                    {
                        // 0  0-1 2-3
                        s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, t, useHalf);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, t, useHalf);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    }
                    break;
                case 6:  // 0110
                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, t, useHalf);
                    break;
                case 7:  // 0111
                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    break;
                case 8:  // 1000
                         // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    break;
                case 9:  // 1001

                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, t, useHalf);
                    break;
                case 10:  // 1010  !!!
                    avg = (m_mat0[r][c] + m_mat0[r][c + 1] + m_mat0[r + 1][c + 1] + m_mat0[r + 1][c]) / 4.0;
                    num10++;
                    if (avg > t)
                    {
                        ///   //  0-1   2-3
                        // 0

                        s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, t, useHalf);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, t, useHalf);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    }
                    else
                    {
                        // 0-3   1-2
                        s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, t, useHalf);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, t, useHalf);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    }
                    break;
                case 11:  // 1011
                          // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, t, useHalf);
                    // 1
                    s.p1 = CALC_LINE_1_INTER(r, c, t, useHalf);
                    break;
                case 12:  // 1100
                          // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, t, useHalf);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
                    break;
                case 13:  // 1101
                          // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, t, useHalf);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, t, useHalf);
                    break;
                case 14:  // 1110

                    // 2
                    s.p0 = CALC_LINE_2_INTER(r, c, t, useHalf);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, t, useHalf);
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
    SLOG_DEBUG("{}", SegNum / 2)
    m_l2seg[level] = lines;
}

void MatchingSquares::TraceRing()
{
    for (auto& [l, _] : m_l2seg)
    {
        TraceLine(l);
    }
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
std::vector<suLine> MatchingSquares::TraceLine(const int& level)
{
    std::vector<suLine> ret;
    auto& segments = m_l2seg[level];
    if (0)
    {// 直接返回
        for (auto& s : segments)
        {
            suLine l;
            l.push_back(s.p0);
            l.push_back(s.p1);
            ret.emplace_back(l);
        }
        return ret;
    }
    
   
    std::vector<std::list<suPoint>> lineParts;
    std::vector<int8_t> mark(segments.size(), 0);
    
    int markNum = 0;
    while (markNum < segments.size())
    {
        std::list<suPoint> tmpLine;
        for (int i = 0; i < mark.size(); ++i)
        {
            if (!mark[i])
            {
                tmpLine.push_back(segments[i].p0);
                tmpLine.push_back(segments[i].p1);
                mark[i] = 1;
                markNum++;
                break;
            }
        }
        if (tmpLine.empty())
        {
            break;
        }
        size_t lastSize = tmpLine.size();
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
                    markNum++;
                    continue;
                }
                np = MatchPoint(segments[i], p1);
                if (np)
                {  // 与末尾的点能匹配上, 就在最后添加一个点
                    tmpLine.push_back(*np);
                    mark[i] = 1;
                    markNum++;
                    continue;
                }
            }
            if (tmpLine.size() == lastSize)
            {
                break;
            }
            lastSize = tmpLine.size();
        }
        
        lineParts.push_back(tmpLine);
    }
    SLOG_DEBUG("总计 {}, 已处理: {}", segments.size(), markNum)
    SLOG_DEBUG("总共{}线", lineParts.size())

    // 转为suLine
    for (auto& r : lineParts)
    {
        if (r.size() < 2)
        {
            SLOG_WARN("线错误")
            continue;
        }
        if (!PointEQ(r.front(), r.back()))
        {
            // SLOG_WARN("首尾不连接")
            // continue;
        }
        suLine l;
        for (auto& p : r)
        {
            l.push_back(p);
        }
        ret.push_back(l);
    }
    SLOG_DEBUG("总计: {} 条线", ret.size())
    return ret;
}



suPoint MatchingSquares::CALC_LINE_0_INTER(const int& r, const int& c, const float& t, const bool& useHalf)
{
    SegNum++;
    suPoint p;
    float l = m_mat0.at(r, c);
    float h = m_mat0.at(r, c + 1);
    p.y = r;
    if (useHalf)
    {
        p.x = c + 0.5;
    }
    else
    {
        p.x = c + (t - l) / (h - l);
    }

    return p;
}

suPoint MatchingSquares::CALC_LINE_1_INTER(const int& r, const int& c, const float& t, const bool& useHalf)
{
    SegNum++;
    suPoint p;
    float l = m_mat0.at(r, c + 1);
    float h = m_mat0.at(r + 1, c + 1);
    if (useHalf)
    {
        p.y = r + 0.5;
    }
    else
    {
        p.y = r + (t - l) / (h - l);
    }

    p.x = c + 1;
    return p;
}

suPoint MatchingSquares::CALC_LINE_2_INTER(const int& r, const int& c, const float& t, const bool& useHalf)
{
    SegNum++;
    suPoint p;
    float l = m_mat0.at(r + 1, c);
    float h = m_mat0.at(r + 1, c + 1);
    // 存在关系  l < t < h
    p.y = r + 1;
    if (useHalf)
    {
        p.x = c + 0.5;
    }
    else
    {
        p.x = c + (t - l) / (h - l);
    }

    return p;
}

suPoint MatchingSquares::CALC_LINE_3_INTER(const int& r, const int& c, const float& t, const bool& useHalf)
{
    SegNum++;
    suPoint p;
    float l = m_mat0.at(r, c);
    float h = m_mat0.at(r + 1, c);
    if (useHalf)
    {
        p.y = r + 0.5;
    }
    else
    {
        p.y = r + (t - l) / (h - l);
    }
    p.x = c;
    return p;
}
