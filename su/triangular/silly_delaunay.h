/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/24 11:53
 * @version: 1.0.1
 * @description: 德劳内 Bowyer-Watson algorithm C++ implementation of http://paulbourke.net/papers/triangulate .
 */
#ifndef SILLY_UTILS_SILLY_DELAUNAY_H
#define SILLY_UTILS_SILLY_DELAUNAY_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#ifndef REAL_EPSILON
#define REAL_EPSILON 1.192092896e-07F /* FLT_EPSILON */
#endif
#define sqrt3 1.732050808F

#if defined(IS_LINUX) || defined(IS_APPLE)
#define REAL_EPSILON 1.192092896e-07F
#endif

#include <cairo/cairo.h>

/// <summary>
/// Bowyer-Watson 算法实现的三角化,目前效率有点问题
/// 目前采用逐点插入方式生成的Delaunay三角网的算法主要基于Bowyer-Watson算法，Bowyer-Watson算法的主要步骤如下：
/// 1）建立初始三角网格：针对给定的点集V, 找到一个包含该点集的矩形R, 我们称R为辅助窗口。连接R的任意一条对角线，形成两个三角形，作为初始Delaunay三角网格。
/// 2）逐点插入：假设目前已经有一个Delaunay三角网格T, 现在在它里面再插入一个点P, 需要找到该点P所在的三角形。从P所在的三角形开始，
/// 搜索该三角形的邻近三角形，并进行空外接圆检测。找到外接圆包含点P的所有的三角形并删除这些三角形，形成一个包含P的多边形空腔，
/// 我们称之为Delaunay空腔。然后连接P与Delaunay腔的每一个顶点，形成新的Delaunay三角网格。
///	3）删除辅助窗口R:重复步骤2）, 当点集V中所有点都已经插入到三角形网格中后，将顶点包含辅助窗口R的三角形全部删除。
/// 在这些步骤中，快速定位点所在的三角形、确定点的影响并构建Delaunay腔的过程是每插入一个点都会进行的。随着点数的增加，三角形数目增加很快，因此缩短这两个过程的计算时间，是提高算法效率的关键。
/// </summary>

struct d_point
{
    size_t i;  // 点的索引,帮助下面快速删除重复边
    double x, y;
    double v;

    d_point() : x{0}, y{0}, v{0}, i{0}
    {
    }

    d_point(double _x, double _y, double _v, size_t _i) : x{_x}, y{_y}, v{_v}, i{_i}
    {
    }

    bool operator==(const d_point& other) const
    {
        // return (other.x == x && other.y == y);
        return other.i == i;
    }

    bool operator!=(const d_point& other) const
    {
        return !operator==(other);
    }

    d_point& operator=(const d_point& other)
    {
        this->x = other.x;
        this->y = other.y;
        this->v = other.v;
        this->i = other.i;
        return *this;
    }
};

struct d_edge
{
    d_point p0, p1;
    // i0 为 p0 和p1中的较小的那个序号
    size_t i0{0}, i1{0};

    d_edge()
    {
    }

    d_edge(d_point const& _p0, d_point const& _p1) : p0{_p0}, p1{_p1}
    {
        if (p0.i < p1.i)
        {
            i0 = p0.i;
            i1 = p1.i;
        }
        else
        {
            i1 = p0.i;
            i0 = p1.i;
        }
    }

    bool operator==(const d_edge& other) const
    {
        return i0 == other.i0 && i1 == other.i1;
    }
};

struct d_circle
{
    double x, y, radius;

    d_circle() = default;
};

struct d_triangle
{
    d_point p0, p1, p2;
    d_edge e0, e1, e2;
    d_circle circle;

    d_triangle(const d_point& _p0, const d_point& _p1, const d_point& _p2) : p0{_p0}, p1{_p1}, p2{_p2}, e0{_p0, _p1}, e1{_p1, _p2}, e2{_p0, _p2}, circle{}
    {
        /*  圆心坐标公式
            x=((C1*B2)-(C2*B1))/((A1*B2)-(A2*B1))；
　　			y=((A1*C2)-(A2*C1))/((A1*B2)-(A2*B1))；
        */
        const auto ax = p1.x - p0.x;
        const auto ay = p1.y - p0.y;
        const auto bx = p2.x - p0.x;
        const auto by = p2.y - p0.y;
        if (ax < REAL_EPSILON || ay < REAL_EPSILON || bx < REAL_EPSILON || by < REAL_EPSILON)
        {
            circle.radius = 0;
        }
        const auto m = p1.x * p1.x - p0.x * p0.x + p1.y * p1.y - p0.y * p0.y;
        const auto u = p2.x * p2.x - p0.x * p0.x + p2.y * p2.y - p0.y * p0.y;
        const auto s = 1. / (2. * (ax * by - ay * bx));
        circle.x = ((p2.y - p0.y) * m + (p0.y - p1.y) * u) * s;
        circle.y = ((p0.x - p2.x) * m + (p1.x - p0.x) * u) * s;

        const auto dx = p0.x - circle.x;
        const auto dy = p0.y - circle.y;
        circle.radius = dx * dx + dy * dy;
    }
};

struct d_delaunay
{
    std::vector<d_triangle> triangles;
    std::vector<d_edge> edges;
};

class silly_delaunay
{
    bool set_data(const std::vector<d_point>& points);

    bool triangulate();

  public:
    d_delaunay d;

  public:
    double xmin, xmax, ymin, ymax;
    std::vector<d_point> n_points;
};

#endif  // SILLY_UTILS_SILLY_DELAUNAY_H
