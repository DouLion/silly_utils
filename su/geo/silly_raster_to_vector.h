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
    silly_point f; // from point
    silly_point t; // to point
    int traced{0};
};

struct silly_trace_node
{
    double val{0};
    silly_point p;
    int traced{0};  // 是否被跟踪过
    int cv{0}; // 类型
    int tc{1};// trace count 被追踪次数, 最多2次
    int great{0};// 是否超过阈值
    std::vector<trace_line_segment> segments;
};



struct trace_square_point
{
    silly_point p;
    double v;
};

class silly_vectorizer
{
  public:

    silly_vectorizer() = default;

    /// <summary>
    /// 设置网格点数据
    /// </summary>
    /// <param name="points"></param>
    /// <returns></returns>
    void set(const std::vector<trace_square_point>& points);
    
    /// <summary>
    /// 标记所有大于threshold的点,为提取边界做准备
    /// </summary>
    /// <returns></returns>
    void mark();

    /// <summary>
    /// 标记出所有边界
    /// </summary>
    void find_edge();

    /// <summary>
    /// 追踪边界,闭合环
    /// </summary>
    /// <returns></returns>
    std::vector<std::vector<silly_point>> trace_ring();


    /// <summary>
    /// 追踪边界,线
    /// </summary>
    /// <returns></returns>
    std::vector<std::vector<silly_point>> trace_line();


    // 坐标插值

    void interpolation(const silly_trace_node &pv1, const silly_trace_node &pv2,  silly_point &point);


    void trace_one_line(int r0l, int c0l, silly_ring &ring);

    static bool point_in_ring(const silly_point& point, const silly_ring &ring);

    std::vector<std::vector<silly_point>>  trace_all_lines();
    std::vector<silly_poly>  trace_all_rings();

  public:
    int m_width{1220};
    int m_height{720};
    double m_xdelta{0.05};
    double m_ydelta{0.05};
    double m_left{74.};
    double m_top{54.};
    double m_right{135};
    double m_bottom{18.};

    int m_interpolation_mode{1}; // 1 精确插值, 2 取中心点


    double m_theshold;
    double m_fill{0.}; // 填充值
    bool m_desc{false};  // 数据大小倒叙排列
    int m_smooth{5};  // 3个点之间贝塞尔插值 n 个点
    int m_ignore_count{5};    // 忽略点数小于一定值的面, 需要在抽稀或者其他简化面的手段之前使用

    std::vector<std::vector<silly_trace_node>> m_mat;
};

#endif  // SILLY_UTILS_SILLY_RASTER_TO_VECTOR_H
