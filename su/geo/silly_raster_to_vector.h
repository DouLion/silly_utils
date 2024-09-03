/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/6 16:14
 * @version: 1.0.1
 * @description: Marching squares 算法 扩展到3维 Marching Cubes
 * 参照 https://blog.csdn.net/whuawell/article/details/74998280
 */
#ifndef SILLY_UTILS_SILLY_RASTER_TO_VECTOR_H
#define SILLY_UTILS_SILLY_RASTER_TO_VECTOR_H

#include <geo/silly_geo.h>

/// 线段
struct trace_line_segment
{
    silly_point f;  // from point
    silly_point t;  // to point
    int traced{0};
};

struct silly_trace_node
{
    double val{0};
    silly_point p;
    int traced{0};  // 是否被跟踪过
    int cv{0};      // 类型
    int tc{1};      // trace count 被追踪次数, 最多2次
    int great{0};   // 是否超过阈值
    std::vector<trace_line_segment> segments;
};

struct trace_square_point
{
    silly_point p;
    double v;
};

struct trace_grid_info
{
    double xdelta{0.05};
    double ydelta{0.05};
    double left{74.};
    double top{54.};
    double right{135};
    double bottom{18.};
};

struct trace_algo_info
{
    double threshold;
    double fill{0};
    int interpolation_mode{2};
    int smooth{5};
    int ignore_count{5};
};

class silly_vectorizer
{
  public:
    silly_vectorizer() = default;

    silly_vectorizer(const silly_vectorizer& right);
    silly_vectorizer& operator=(const silly_vectorizer& right);

    /// <summary>
    /// 矢量化所有面
    /// </summary>
    /// <param name="points"></param>
    /// <param name="t">过滤阈值</param>
    /// <returns></returns>
    std::vector<silly_poly> vectorize(const std::vector<trace_square_point>& points, const double& t);

    /// <summary>
    /// 设置网格点信息
    /// </summary>
    /// <param name="points"></param>
    /// <returns></returns>
    void set(const trace_grid_info& info);

    /// <summary>
    /// 设置矢量化参数信息
    /// </summary>
    /// <param name="points"></param>
    /// <returns></returns>
    void set(const trace_algo_info& info);

    /// <summary>
    /// 设置网格点数据
    /// </summary>
    /// <param name="points"></param>
    /// <returns></returns>
    void set(const std::vector<trace_square_point>& points);

    void set(const std::vector<trace_square_point>& points, const double& t);

    // useless
    void set(const std::vector<trace_square_point>& points, const double& low, const double& high);

    std::vector<silly_poly> smooth_poly(const std::vector<silly_poly>& polys);
    std::vector<silly_poly> simplify_poly_less_angle(const std::vector<silly_poly>& polys, const double& angle);
    std::vector<silly_poly> simplify_poly_same_slope(const std::vector<silly_poly>& polys);
    std::vector<silly_poly> simplify_poly_mid_point(const std::vector<silly_poly>& polys);

    /// <summary>
    /// 消除斜率相同的连续点, 以简化环
    /// </summary>
    /// <param name="ring"></param>
    /// <returns></returns>
    silly_ring simplify_ring_same_slope(const silly_ring& ring);

    /// <summary>
    /// 跟具连续两个线段的夹角差小于一定一定角度,简化环
    /// </summary>
    /// <param name="ring"></param>
    /// <param name="angle"></param>
    /// <returns></returns>
    silly_ring simplify_ring_less_angle(const silly_ring& ring, double angle);

    /// <summary>
    /// 根据具两个连续点与参照点的夹角差小于一定一定角度,简化环
    /// </summary>
    /// <param name="ring"></param>
    /// <param name="angle"></param>
    /// <returns></returns>
    silly_ring simplify_ring_less_angle_1(const silly_ring& ring, double angle);
    silly_ring smooth_ring(const silly_ring& ring);

    /// <summary>
    /// 道格拉斯曲线抽稀
    /// </summary>
    /// <param name="ring"></param>
    /// <param name="dist">距离容差, 不确定具体含义,但是调整可以获得更好的效果</param>
    /// <returns></returns>
    silly_ring simplify_ring_douglas(const silly_ring& ring, double dist);

    /// <summary>
    /// 标记所有大于threshold的点,为提取边界做准备
    /// </summary>
    /// <returns></returns>
    void mark();

    void mark_edge(int r, int c, int tp);

    /// <summary>
    /// 标记出所有边界
    /// </summary>
    void find_edge();

    // 坐标插值

    void interpolation(const silly_trace_node& pv1, const silly_trace_node& pv2, silly_point& point);

    void trace_one_line(int r0l, int c0l, silly_ring& ring);

    bool recurse_trace_line(int r, int c, silly_ring& ring);

    static bool point_in_ring(const silly_point& point, const silly_ring& ring, const double& maxx = 1.e8);

    std::vector<std::vector<silly_point>> trace_all_lines();
    std::vector<silly_poly> trace_all_polys();

    size_t width() const;
    size_t height() const;

  private:
    void fill_mat();

  private:
    int m_width{0};
    int m_height{0};
    double m_xdelta{0.05};
    double m_ydelta{0.05};
    double m_left{74.};
    double m_top{54.};
    double m_right{135};
    double m_bottom{18.};

    int m_interpolation_mode{2};  // 1 精确插值, 2 取中心点 3 双阈值

    double m_threshold_l{0.};  // 低阈值
    double m_threshold_h{0.};  // 高阈值
    double m_fill{0.};         // 填充值
    int m_smooth{5};           // 3个点之间贝塞尔插值 n 个点
    int m_ignore_count{5};     // 忽略点数小于一定值的面, 需要在抽稀或者其他简化面的手段之前使用

    std::vector<std::vector<silly_trace_node>> m_mat;
};

#endif  // SILLY_UTILS_SILLY_RASTER_TO_VECTOR_H
