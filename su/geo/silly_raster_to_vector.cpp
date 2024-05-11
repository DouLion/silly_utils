//
// Created by dell on 2023/12/6.
//

#include "silly_raster_to_vector.h"


#define RECURSION_TRACE_LINE(r, c)                                     \
    if (r > -1 && c > -1 && r < b_w_mat->height && c < b_w_mat->width) \
    {                                                                  \
        for (auto &segment : b_w_mat->mat[r][c].segments)              \
        {                                                              \
            if (!segment.traced)                                       \
            {                                                          \
                if (ring.points.back() == segment.from_p)              \
                {                                                      \
                    ring.points.push_back(segment.to_p);               \
                    segment.traced = 1;                                \
                    trace_one_line(b_w_mat, r, c, ring);               \
                    return;                                            \
                }                                                      \
                if (ring.points.back() == segment.to_p)                \
                {                                                      \
                    ring.points.push_back(segment.from_p);             \
                    segment.traced = 1;                                \
                    trace_one_line(b_w_mat, r, c, ring);               \
                    return;                                            \
                }                                                      \
            }                                                          \
        }                                                              \
    }

#define CALC_LINE_0_INTER(m, r, c, t, p) interpolation(m[r][c].pv, m[r][c + 1].pv, threshold, p)
#define CALC_LINE_1_INTER(m, r, c, t, p) interpolation(m[r][c + 1].pv, m[r + 1][c + 1].pv, threshold, p)
#define CALC_LINE_2_INTER(m, r, c, t, p) interpolation(m[r + 1][c].pv, m[r + 1][c + 1].pv, threshold, p)
#define CALC_LINE_3_INTER(m, r, c, t, p) interpolation(m[r][c].pv, m[r + 1][c].pv, threshold, p)

void silly_vectorizer::mark_all(black_white_mat &b_w_mat, double threshold)
{
    for (size_t r = 0; r < m_height + 1; ++r)
    {
        for (size_t c = 0; c < m_width + 1; ++c)
        {
            // 新疆有些网格点有缺省
            b_w_mat.mat[r][c].pv.lgtd = m_left - m_x_step + c * m_x_step;
            b_w_mat.mat[r][c].pv.lttd = m_top + m_y_step - r * m_y_step;
            b_w_mat.mat[r][c].cv = (b_w_mat.mat[r][c].is_white << 3) | (b_w_mat.mat[r][c + 1].is_white << 2) | (b_w_mat.mat[r + 1][c + 1].is_white << 1) | b_w_mat.mat[r + 1][c].is_white;
        }
    }
    double avg;
    // 插值计算完所有可能的边界
    for (int r = 0; r < m_height + 2; ++r)
    {
        for (int c = 0; c < m_width + 2; ++c)
        {
            switch (b_w_mat.mat[r][c].cv)
            {
                case 1:  // 0001
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 2
                    CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 3
                    CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);

                    break;
                case 2:  // 0010
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 1
                    CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 2
                    CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 3:  // 0011
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 1
                    CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 3
                    CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 4:  // 0100
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 0
                    CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 1
                    CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 5:  // 0101	!!!
                    b_w_mat.mat[r][c].segments.resize(2);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    b_w_mat.mat[r][c].segments[1].traced = 0;
                    avg = (b_w_mat.mat[r][c].pv.value + b_w_mat.mat[r][c + 1].pv.value + b_w_mat.mat[r + 1][c + 1].pv.value + b_w_mat.mat[r + 1][c].pv.value) / 4.0;

                    if (avg > threshold)
                    {
                        ///   //  0-3   1-2
                        // 0
                        CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                        // 1
                        CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].from_p);
                        // 2
                        CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].to_p);
                        // 3
                        CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    }
                    else
                    {
                        b_w_mat.mat[r][c].t1 = 0;
                        // 0  0-1 2-3
                        CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                        // 1
                        CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                        // 2
                        CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].from_p);
                        // 3
                        CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].to_p);
                    }
                    break;
                case 6:  // 0110
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 0
                    CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 2
                    CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 7:  // 0111
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 0
                    CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 3
                    CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 8:  // 1000
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 0
                    CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 3
                    CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 9:  // 1001
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 0
                    CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 2
                    CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 10:  // 1010  !!!
                    b_w_mat.mat[r][c].segments.resize(2);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    b_w_mat.mat[r][c].segments[1].traced = 0;
                    avg = (b_w_mat.mat[r][c].pv.value + b_w_mat.mat[r][c + 1].pv.value + b_w_mat.mat[r + 1][c + 1].pv.value + b_w_mat.mat[r + 1][c].pv.value) / 4.0;

                    if (avg > threshold)
                    {
                        ///   //  0-1   2-3
                        // 0
                        b_w_mat.mat[r][c].t1 = 0;
                        CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                        // 1
                        CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                        // 2
                        CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].from_p);
                        // 3
                        CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].to_p);
                    }
                    else
                    {
                        // 0-3   1-2
                        CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                        // 1
                        CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].from_p);
                        // 2
                        CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[1].to_p);
                        // 3
                        CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    }
                    break;
                case 11:  // 1011
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 0
                    CALC_LINE_0_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 1
                    CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 12:  // 1100
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 1
                    CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 3
                    CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 13:  // 1101
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 1
                    CALC_LINE_1_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 2
                    CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
                case 14:  // 1110
                    b_w_mat.mat[r][c].segments.resize(1);
                    b_w_mat.mat[r][c].segments[0].traced = 0;
                    // 2
                    CALC_LINE_2_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].from_p);
                    // 3
                    CALC_LINE_3_INTER(b_w_mat.mat, r, c, threshold, b_w_mat.mat[r][c].segments[0].to_p);
                    break;
            }
        }
    }
}

std::vector<silly_poly_2> silly_vectorizer::trace_by_threshold(const double &threshold, const silly_grid_record *record)
{
    black_white_mat b_w_mat;
    b_w_mat.create(m_height + 2, m_width + 2);
    // TODO: 计算边界然后插值可能更合适
    if (m_desc)
    {
        // 填充上下两个边界,
        for (int i = 0; i < m_width + 2; ++i)
        {
            b_w_mat.mat[0][i].pv.lgtd = m_left + (i - 1) * m_x_step;
            b_w_mat.mat[0][i].pv.lttd = m_top + m_y_step;
            b_w_mat.mat[m_height + 1][i].pv.lgtd = m_left + (i - 1) * m_x_step;
            b_w_mat.mat[m_height + 1][i].pv.lttd = m_bottom - m_y_step;
        }
        // 填充左右两个边界
        for (int i = 0; i < m_height + 2; ++i)
        {
            b_w_mat.mat[i][0].pv.lgtd = m_left - m_x_step;
            b_w_mat.mat[i][0].pv.lttd = m_top - (i - 1) * m_y_step;
            b_w_mat.mat[i][m_width + 1].pv.lgtd = m_right + m_x_step;
            b_w_mat.mat[i][m_width + 1].pv.lttd = m_top - (i - 1) * m_y_step;
        }

        // 根据阈值标记点的 黑或者白
        for (int i = 0; i < record->num; ++i)
        {
            int x = std::round((record->lgtd[i] - m_left) / m_x_step) + 1;
            int y = std::round((m_top - record->lttd[i]) / m_y_step) + 1;
            b_w_mat.mat[y][x].pv.lgtd = record->lgtd[i];
            b_w_mat.mat[y][x].pv.lttd = record->lttd[i];
            b_w_mat.mat[y][x].pv.value = record->value[i];
            //				b_w_mat.mat[y][x].pv.x = x;
            //				b_w_mat.mat[y][x].pv.y = y;

            if (b_w_mat.mat[y][x].pv.value > threshold)
            {
                b_w_mat.mat[y][x].is_white = 1;
            }
        }
    }
    else
    {
        // 填充上下两个边界,
        for (int i = 0; i < m_width + 2; ++i)
        {
            b_w_mat.mat[0][i].pv.lgtd = m_left + (i - 1) * m_x_step;
            b_w_mat.mat[0][i].pv.lttd = m_top + m_y_step;
            b_w_mat.mat[m_height + 1][i].pv.lgtd = m_left + (i - 1) * m_x_step;
            b_w_mat.mat[m_height + 1][i].pv.lttd = m_bottom - m_y_step;
        }
        // 填充左右两个边界
        for (int i = 0; i < m_height + 2; ++i)
        {
            b_w_mat.mat[i][0].pv.lgtd = m_left - m_x_step;
            b_w_mat.mat[i][0].pv.lttd = m_top - (i - 1) * m_y_step;
            b_w_mat.mat[i][m_width + 1].pv.lgtd = m_right + m_x_step;
            b_w_mat.mat[i][m_width + 1].pv.lttd = m_top - (i - 1) * m_y_step;
        }

        // 根据阈值标记点的 黑或者白
        for (int i = 0; i < record->num; ++i)
        {
            int x = std::round((record->lgtd[i] - m_left) / m_x_step) + 1;
            int y = std::round((m_top - record->lttd[i]) / m_y_step) + 1;
            b_w_mat.mat[y][x].pv.lgtd = record->lgtd[i];
            b_w_mat.mat[y][x].pv.lttd = record->lttd[i];
            b_w_mat.mat[y][x].pv.value = record->value[i];
            //				b_w_mat.mat[y][x].pv.x = x;
            //				b_w_mat.mat[y][x].pv.y = y;

            if (b_w_mat.mat[y][x].pv.value > threshold)
            {
                b_w_mat.mat[y][x].is_white = 1;
            }
        }
    }

    silly_image::png_data png = silly_image::png_utils::create_empty(m_height + 2, m_width + 2, PNG_COLOR_TYPE_GRAY);
    for (int r = 0; r < m_height + 2; ++r)
    {
        for (int c = 0; c < m_width + 2; ++c)
        {
            png.data[r][c] += b_w_mat.mat[r][c].is_white * 200;
        }
    }
    silly_image::png_utils::write("E:/hubei_mark.png", png);
    // 根据算法标记所有点
    mark_all(b_w_mat, threshold);
#ifndef NDEBUG
    std::cout << "START trace ..." << std::endl;
#endif
    std::vector<silly_ring_2> all_path;

    // 追踪边界线,形成完整面
    trace_all_lines(&b_w_mat, all_path);

    // 对面做拓扑分析
    std::vector<silly_poly_2> valid_polys;

    for (auto &ring : all_path)
    {
        if (ring.is_outer)
        {
            silly_poly_2 sp;
            sp.rings.emplace_back(ring);
            valid_polys.emplace_back(sp);
        }
    }
    for (auto &ring : all_path)
    {
        if (!ring.is_outer)
        {
            for (auto &poly : valid_polys)
            {
                if (point_in_ring(ring.points[0].lgtd, ring.points[0].lttd, poly.rings[0]))
                {
                    poly.rings.emplace_back(ring);
                    break;
                }
            }
        }
    }
    // return valid_polys;
    std::vector<silly_poly_2> simple_polys = simplify_poly(valid_polys);
    // #ifndef NDEBUG
    //		std::string result = trans_poly_to_geojson(valid_polys);
    //		FILE* pf = fopen("result.geojson", "wb");
    //		if (pf)
    //		{
    //			fwrite(result.c_str(), 1, result.length(), pf);
    //			fclose(pf);
    //		}
    // #endif
    //
    return simple_polys;
    // return smooth_poly(simple_polys);
}

double silly_vectorizer::bezier(const double &t, double x1, double x2, double x3)
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

void silly_vectorizer::interpolation(const silly_p_value &pv1, const silly_p_value &pv2, const double &threshold, silly_point_2 &point)

{
    // 这个地方被除数一定不会等于0, 如果等于0,一定是前面有些步骤做法不对,或者阈值划分有问题
    point.lgtd = (threshold - pv1.value) * (pv2.lgtd - pv1.lgtd) / (pv2.value - pv1.value) + pv1.lgtd;
    point.lttd = (threshold - pv1.value) * (pv2.lttd - pv1.lttd) / (pv2.value - pv1.value) + pv1.lttd;

    if (point.lgtd < 10 || point.lttd < 10)
    {
        int a = 0;
    }
}

std::vector<silly_poly_2> silly_vectorizer::smooth_poly(const std::vector<silly_poly_2> &polys)
{
    std::vector<silly_poly_2> smooth_polys;
    for (auto poly : polys)
    {
        silly_poly_2 smooth_poly;
        for (auto ring : poly.rings)
        {
            while (ring.points.back() == ring.points.front())
            {
                ring.points.pop_back();
            }
            silly_ring_2 smooth_ring;
            size_t p_size = ring.points.size();
            double x = 0, y = 0;
            for (int j = 0; j < ring.points.size(); j++)
            {
                double bzr_step = 1.0 / m_smooth;
                double t = bzr_step;

                // 取中点作为控制点
                size_t m1 = j + 1, m2 = j + 2;
                m1 = m1 < p_size ? m1 : m1 - p_size;
                m2 = m2 < p_size ? m2 : m2 - p_size;
                double x0 = (ring.points[j].lgtd + ring.points[m1].lgtd) / 2;
                double y0 = (ring.points[j].lttd + ring.points[m1].lttd) / 2;
                double x2 = (ring.points[m2].lgtd + ring.points[m1].lgtd) / 2;
                double y2 = (ring.points[m2].lttd + ring.points[m1].lttd) / 2;

                smooth_ring.points.emplace_back(x0, y0);
                for (int k = 0; k < m_smooth; k++)
                {
                    x = bezier(t, x0, ring.points[m1].lgtd, x2);
                    y = bezier(t, y0, ring.points[m1].lttd, y2);
                    smooth_ring.points.emplace_back(x, y);
                    t += bzr_step;
                }
            }
            smooth_poly.rings.emplace_back(smooth_ring);
        }

        smooth_polys.push_back(smooth_poly);
    }
    return smooth_polys;
}

bool silly_vectorizer::is_less_than_slope(const silly_point_2 &p1, const silly_point_2 &p2, const silly_point_2 &p3, const double angle)
{
    // double slope = 0.1;// std::tan(std::abs(angle));
    double x1_diff = p2.lgtd - p1.lgtd;
    double x2_diff = p3.lgtd - p2.lgtd;
    double y1_diff = p2.lttd - p1.lttd;
    double y2_diff = p3.lttd - p2.lttd;

    // 检查分母是否接近零，避免除以零的情况
    if (std::abs(x1_diff) < FLOAT_IGNORE_DIFF || std::abs(x2_diff) < FLOAT_IGNORE_DIFF)
    {
        if (std::abs(x1_diff - x2_diff) < FLOAT_IGNORE_DIFF)
        {
            return true;  // 两点间几乎平行，视作通过测试
        }
        return false;  // 其他情况下，由于无法计算斜率，返回false
    }

    // 使用std::atan2直接处理象限问题，并避免了不必要的数学常数转换
    double angle1 = std::atan2(y1_diff, x1_diff) * 180.0 / M_PI;
    double angle2 = std::atan2(y2_diff, x2_diff) * 180.0 / M_PI;

    // 考虑角度的周期性，确保不丢失解决方案
    double diff = std::abs(angle1 - angle2);
    if (diff < angle)
    {
        return true;
    }
    else if (diff > 180.0 - angle)
    {
        // 如果两个角度的差值大于180度减去指定角度，则调整角度差，使其小于180度
        return true;
    }

    return false;
}

std::vector<silly_poly_2> silly_vectorizer::simplify_poly(const std::vector<silly_poly_2> &polys, const double &angle /*= 5.0*/)
{
    std::vector<silly_poly_2> simple_polys;
    for (auto poly : polys)
    {
        silly_poly_2 simple_poly;
        for (auto ring : poly.rings)
        {
            while (ring.points.back() == ring.points.front())
            {
                ring.points.pop_back();
            }
            silly_ring_2 simple_ring;
            simple_ring.points.push_back(ring.points[0]);
            size_t p_size = ring.points.size();
            double x = 0, y = 0;
            for (int j = 0; j < ring.points.size(); j++)
            {
                // 取中点作为控制点
                size_t m1 = j + 1, m2 = j + 2;
                m1 = m1 < p_size ? m1 : m1 - p_size;
                m2 = m2 < p_size ? m2 : m2 - p_size;
                if (!is_less_than_slope(simple_ring.points.back(), ring.points[m1], ring.points[m2], angle))  // 角度相差过大就把当中间点塞进去
                {
                    simple_ring.points.emplace_back(ring.points[m1]);
                }
            }
            simple_poly.rings.emplace_back(simple_ring);
        }

        simple_polys.emplace_back(simple_poly);
    }

    return simple_polys;
}

void silly_vectorizer::trace_one_line(black_white_mat *b_w_mat, int r0l, int c0l, silly_ring_2 &ring)
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

bool silly_vectorizer::point_in_ring(const double &x, const double &y, const silly_ring_2 &r)
{
    size_t i, j;
    double d;
    int c = 0;
    size_t num = r.points.size();
    for (i = 0, j = num - 1; i < num; j = i++)
    {
        d = (r.points[j].lgtd - r.points[i].lgtd) * (y - r.points[i].lttd) / (r.points[j].lttd - r.points[i].lttd) + r.points[i].lgtd;
        if (x == d)
            return 0;
        if ((((r.points[i].lttd <= y) && (y < r.points[j].lttd) || ((r.points[j].lttd <= y) && (y < r.points[i].lttd))) && (x < d)))
            c = !c;
    }
    return c;
}

void silly_vectorizer::trace_all_lines(black_white_mat *b_w_mat, std::vector<silly_ring_2> &all_path)

{
    bool has_not_traced = true;
    int count = 0;
    while (has_not_traced)
    {
        has_not_traced = false;
        silly_ring_2 ring;
        silly_p_value first_w_point;  // TODO:
        // 检查第一个未被追踪到的点
        for (int r = 1; r < m_height + 2 && !has_not_traced; ++r)
        {
            for (int c = 1; c < m_width + 2 && !has_not_traced; ++c)
            {
                switch (b_w_mat->mat[r][c].cv)
                {
                    case 0:
                    case 15:
                        break;
                    default:
                        for (auto &segment : b_w_mat->mat[r][c].segments)
                        {
                            if (!segment.traced)
                            {
                                ring.points.push_back(segment.from_p);
                                ring.points.push_back(segment.to_p);
                                segment.traced = 1;

                                // 递归查找下一个点
                                trace_one_line(b_w_mat, r, c, ring);
                                switch (b_w_mat->mat[r][c].cv)
                                {
                                    case 14:
                                    case 13:
                                    case 11:
                                    case 8:
                                    case 12:
                                    case 9:
                                        // 取当前点就行
                                        first_w_point = b_w_mat->mat[r][c].pv;
                                        break;
                                    case 10:
                                    case 5:
                                        if (b_w_mat->mat[r][c].t1)
                                        {
                                            first_w_point = b_w_mat->mat[r][c].pv;
                                        }
                                        else
                                        {
                                            first_w_point = b_w_mat->mat[r][c + 1].pv;
                                        }
                                        break;
                                    case 7:
                                    case 4:
                                    case 6:
                                        first_w_point = b_w_mat->mat[r][c + 1].pv;
                                        // 右上
                                        break;
                                    case 1:
                                    case 3:
                                        first_w_point = b_w_mat->mat[r + 1][c].pv;
                                        // 左下
                                        break;
                                    case 2:
                                        first_w_point = b_w_mat->mat[r + 1][c + 1].pv;
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
                ring.points.push_back(ring.points.front());
            }
            // 点在面内判断
            if (!point_in_ring(first_w_point.lgtd, first_w_point.lttd, ring))
            {
                ring.is_outer = 0;
            }
            if (!first_w_point.value)  // 第一个点是外面的点
            {
                ring.is_outer = 1 - ring.is_outer;
            }
            // std::cout << "Point num: " << ring.points.size() << ", is inner: " << ring.is_outer<< std::endl;

            all_path.push_back(ring);
        }
    }
}
