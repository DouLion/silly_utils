//
// Created by dell on 2023/12/6.
//

#include "silly_raster_to_vector.h"
#include <log/silly_log.h>
#include <geo/vacuate/silly_vacuate.h>
#include <pool/silly_thread_pool.h>

static bool PointEQ(suPoint* p1, suPoint* p2)
{
    return std::abs(p1->x - p2->x) < 0.01 && std::abs(p1->z - p2->z) < 0.01;
}

static bool SegmentEQ(suSegment* s1, suSegment* s2)
{
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
    if (mat.row() < 5 || mat.col() < 5)
    {
        SLOG_ERROR("????这么小的矩阵你想干啥????")
        return;
    }
    m_mat0 = mat.copy();
    m_height = m_mat0.row();
    m_width = m_mat0.col();
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
    return {};
}
std::vector<suPoly> MatchingSquares::GetPolys(const ReadParam& param)
{
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
    int8_t* pwb = wb.data();
    float* p0 = m_mat0.data();
    size_t i = 0;
    float t = m_levels[level];
    while (i < m_height * m_width)
    {
        if (p0[i] >= t)
        {
            pwb[i] = 1;
        }
        i++;
    }
    m_l2wb[level] = wb;
}
bool useHalf = true;
[[nodiscard]] suPoint CALC_LINE_0_INTER(const int& r, const int& c, suMatrix<int8_t>* mat, const float& t)
{
    suPoint p;
    float l = mat->at(r, c);
    float h = mat->at(r, c + 1);
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

[[nodiscard]] suPoint CALC_LINE_1_INTER(const int& r, const int& c, suMatrix<int8_t>* mat, const float& t)
{
    suPoint p;
    float l = mat->at(r, c + 1);
    float h = mat->at(r + 1, c + 1);
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

[[nodiscard]] suPoint CALC_LINE_2_INTER(const int& r, const int& c, suMatrix<int8_t>* mat, const float& t)
{
    suPoint p;
    float l = mat->at(r + 1, c);
    float h = mat->at(r + 1, c + 1);
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

[[nodiscard]] suPoint CALC_LINE_3_INTER(const int& r, const int& c, suMatrix<int8_t>* mat, const float& t)
{
    suPoint p;
    float l = mat->at(r, c);
    float h = mat->at(r + 1, c);
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

void MatchingSquares::TraceWhiteBlack(const int& level)
{
    auto& wb = m_l2wb[level];
    float t = m_levels[level];
    // 先分类

    int markNum = 0;
#if 1
    for (size_t r = 1; r < m_height - 1; r++)
    {
        for (size_t c = 1; c < m_width - 1; c++)
        {
#else
    for (size_t r = 5; r < 10; r++)
    {
        for (size_t c = 163; c < 168; c++)
        {
#endif
            // r 165 c 7

            int8_t& v = wb[r][c];
            if (r < 5 || r > 7 || c < 163 || c > 168)
            {
                v = 0;
                continue;
            }
            //std::cout << m_mat0[r][c] << " ";
            if (v == 0)
            {
                continue;
            }

            wb[r][c] = (wb[r][c] << 3) | (wb[r][c + 1] << 2) | (wb[r + 1][c + 1] << 1) | wb[r + 1][c];
            markNum++;
        }
        //std::cout << std::endl;
    }
    std::cout << markNum << std::endl;
    // 超过16表示已经标记过
    std::vector<suSegment> lines;
    for (size_t r = 1; r < m_height - 1; r++)
    {
        for (size_t c = 1; c < m_width - 1; c++)
        {
            suSegment s, s2;
            float avg = 0;
            int8_t& v = wb[r][c];
            switch (v)
            {
                case 1:  // 0001

                    // 2
                    s.p0 = CALC_LINE_2_INTER(r, c, &wb, t);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, &wb, t);

                    break;
                case 2:  // 0010

                    // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, &wb, t);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, &wb, t);
                    break;
                case 3:  // 0011

                    // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, &wb, t);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    break;
                case 4:  // 0100

                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                    // 1
                    s.p1 = CALC_LINE_1_INTER(r, c, &wb, t);
                    break;
                case 5:  // 0101	!!!
                    avg = (m_mat0[r][c] + m_mat0[r][c + 1] + m_mat0[r + 1][c + 1] + m_mat0[r + 1][c]) / 4.0;

                    if (avg > t)
                    {
                        ///   //  0-3   1-2
                        // 0
                        s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, &wb, t);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, &wb, t);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    }
                    else
                    {
                        // 0  0-1 2-3
                        s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, &wb, t);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, &wb, t);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    }
                    break;
                case 6:  // 0110

                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, &wb, t);
                    break;
                case 7:  // 0111

                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    break;
                case 8:  // 1000
                         // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    break;
                case 9:  // 1001

                    // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, &wb, t);
                    break;
                case 10:  // 1010  !!!
                    avg = (m_mat0[r][c] + m_mat0[r][c + 1] + m_mat0[r + 1][c + 1] + m_mat0[r + 1][c]) / 4.0;

                    if (avg > t)
                    {
                        ///   //  0-1   2-3
                        // 0

                        s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, &wb, t);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, &wb, t);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    }
                    else
                    {
                        // 0-3   1-2
                        s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                        // 1
                        s.p1 = CALC_LINE_1_INTER(r, c, &wb, t);
                        // 2
                        s2.p0 = CALC_LINE_2_INTER(r, c, &wb, t);
                        // 3
                        s2.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    }
                    break;
                case 11:  // 1011
                          // 0
                    s.p0 = CALC_LINE_0_INTER(r, c, &wb, t);
                    // 1
                    s.p1 = CALC_LINE_1_INTER(r, c, &wb, t);
                    break;
                case 12:  // 1100
                          // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, &wb, t);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
                    break;
                case 13:  // 1101
                          // 1
                    s.p0 = CALC_LINE_1_INTER(r, c, &wb, t);
                    // 2
                    s.p1 = CALC_LINE_2_INTER(r, c, &wb, t);
                    break;
                case 14:  // 1110

                    // 2
                    s.p0 = CALC_LINE_2_INTER(r, c, &wb, t);
                    // 3
                    s.p1 = CALC_LINE_3_INTER(r, c, &wb, t);
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
    if (lines.empty())
    {
        SLOG_WARN("未追踪到")
    }
    SLOG_INFO("线段数量: {}", lines.size())
    m_l2seg[level] = lines;
}