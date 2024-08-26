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

#include <su_marco.h>
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
    int width{1220};
    int height{720};
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

    /// <summary>
    /// 标记所有大于threshold的点,为提取边界做准备
    /// </summary>
    /// <returns></returns>
    void mark();

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

  private:
    void fill_mat();

  private:
    int m_width{1220};
    int m_height{720};
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
