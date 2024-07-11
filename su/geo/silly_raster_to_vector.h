/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/6 16:14
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_RASTER_TO_VECTOR_H
#define SILLY_UTILS_SILLY_RASTER_TO_VECTOR_H

#include <su_marco.h>
#include <json/json.h>

#include <image/png_utils.h>

#define FLOAT_IGNORE_DIFF 0.00000001

#define POS_MAX_VALUE 1e10

struct silly_point_2  // 普通坐标点
{
    silly_point_2() = default;

    silly_point_2(double x, double y)
    {
        lgtd = x;
        lttd = y;
    }

    double lgtd{0};
    double lttd{0};

    silly_point_2 &operator=(const silly_point_2 &point) = default;

    bool operator==(const silly_point_2 &point) const
    {
        return std::abs(point.lgtd - this->lgtd) < FLOAT_IGNORE_DIFF && std::abs(point.lttd - this->lttd) < FLOAT_IGNORE_DIFF;
    }

    bool operator!=(const silly_point_2 &point) const
    {
        return std::abs(point.lgtd - this->lgtd) > FLOAT_IGNORE_DIFF || std::abs(point.lttd - this->lttd) > FLOAT_IGNORE_DIFF;
    }
};

struct silly_segment
{
    silly_point_2 from_p;  // 预设计一个方向
    silly_point_2 to_p;
    int traced{1};  // 是否被跟踪过
};

struct silly_ring_2
{
    std::vector<silly_point_2> points;
    int is_outer{1};
};

struct silly_poly_2
{
    std::vector<silly_ring_2> rings;
};

struct silly_p_value : public silly_point_2
{
    double value{0};
    silly_p_value operator=(const silly_p_value &spv)
    {
        this->value = spv.value;
        this->lgtd = spv.lgtd;
        this->lttd = spv.lttd;
        return *this;
    }
};

struct ceil_mark
{
    int cv{0};        // 特征值 characteristic value
    int is_white{0};  // 是否大于阈值
    silly_p_value pv;
    int t1 = {1};                         // 第一个点在阈值点之上
    std::vector<silly_segment> segments;  // 可能有多条线段
};

struct black_white_mat
{
    std::vector<std::vector<ceil_mark>> mat;
    int height{0};
    int width{0};

    void create(const int &h, const int &w)
    {
        height = h;
        width = w;
        mat.resize(height);
        for (auto &r : mat)
        {
            r.resize(width);
        }
    }
};

/// <summary>
/// 网格点记录
/// </summary>
struct silly_grid_record
{
    size_t num{0};
    double *lgtd{nullptr};
    double *lttd{nullptr};
    double *value{nullptr};
    double left{POS_MAX_VALUE};
    double top{-POS_MAX_VALUE};
    double right{-POS_MAX_VALUE};
    double bottom{POS_MAX_VALUE};

    void create(size_t size)
    {
        num = size;
        lgtd = (double *)malloc(num * sizeof(double));
        lttd = (double *)malloc(num * sizeof(double));
        value = (double *)malloc(num * sizeof(double));
    }

    void destroy()
    {
        if (lgtd)
        {
            free(lgtd);
            lgtd = nullptr;
        }
        if (lttd)
        {
            free(lttd);
            lttd = nullptr;
        }
        if (value)
        {
            free(value);
            value = nullptr;
        }
    }
};

class silly_vectorizer
{
  public:
    int m_width{0};
    int m_height{0};

    // 这些属性 以 左下角为坐标原点
    double m_x_step{0.05};
    double m_y_step{0.05};
    double m_left{0};
    double m_top{0};
    double m_right{0};
    double m_bottom{0};

    bool m_desc{false};  // 数据大小倒叙排列

    int m_smooth{5};  // 3个点之间贝塞尔插值 n 个点

    int m_ignore_count{5};    // 忽略点数小于一定值的面, 需要在抽稀或者其他简化面的手段之前使用
    double m_ignore_area{0};  // 忽略面积小于一定值的面

    /// <summary>
    /// 标记所有大于threshold的点,为提取边界做准备
    /// </summary>
    /// <param name="b_w_mat"></param>
    /// <param name="threshold"></param>
    void mark_all(black_white_mat &b_w_mat, double threshold);

    /// <summary>
    /// 追踪边界
    /// </summary>
    /// <param name="threshold"></param>
    /// <param name="record"></param>
    /// <returns></returns>
    std::vector<silly_poly_2> trace_by_threshold(const double &threshold, const silly_grid_record *record);

    static double bezier(const double &t, double x1, double x2, double x3);

    // 坐标插值

    static void interpolation(const silly_p_value &pv1, const silly_p_value &pv2, const double &threshold, silly_point_2 &point);

    /// <summary>
    /// 平滑矢量线,使用贝塞尔插值
    /// </summary>
    /// <param name="polys"></param>
    /// <returns></returns>
    std::vector<silly_poly_2> smooth_poly(const std::vector<silly_poly_2> &polys);

    /// <summary>
    /// 判断连续的两个线段,斜率是否相似
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="p3"></param>
    /// <param name="slope"></param>
    /// <returns></returns>
    bool is_less_than_slope(const silly_point_2 &p1, const silly_point_2 &p2, const silly_point_2 &p3, const double angle);

    /// <summary>
    /// 根据角度差简化边界线
    /// </summary>
    /// <param name="polys"></param>
    /// <param name="diff"></param>
    /// <returns></returns>
    std::vector<silly_poly_2> simplify_poly(const std::vector<silly_poly_2> &polys, const double &angle = 5.0);

    void trace_one_line(black_white_mat *b_w_mat, int r0l, int c0l, silly_ring_2 &ring);

    static bool point_in_ring(const double &x, const double &y, const silly_ring_2 &r);

    void trace_all_lines(black_white_mat *b_w_mat, std::vector<silly_ring_2> &all_path);
};

#endif  // SILLY_UTILS_SILLY_RASTER_TO_VECTOR_H
