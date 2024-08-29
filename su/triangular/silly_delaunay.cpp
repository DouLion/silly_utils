//
// Created by dell on 2024/1/24.
//

#include "silly_delaunay.h"

bool silly_delaunay::set_data(const std::vector<d_point>& points)
{
    size_t pnum = points.size();
    if (pnum < 3)
    {
        return false;
    }
    xmin = points[0].x;
    xmax = xmin;
    ymin = points[0].y;
    ymax = ymin;
    for (auto const& pt : points)
    {
        xmin = std::min(xmin, pt.x);
        xmax = std::max(xmax, pt.x);
        ymin = std::min(ymin, pt.y);
        ymax = std::max(ymax, pt.y);
    }

    const auto dx = xmax - xmin;
    const auto dy = ymax - ymin;

    xmin -= (xmax - xmin) / 10.;  // 外扩1/10;
    ymin -= (ymax - ymin) / 10.;
    xmax += (xmax - xmin) / 10.;
    ymax += (ymax - ymin) / 10.;

    /* Init d_delaunay triangulation. */

    const auto p0 = d_point{xmin, ymin, 0, pnum};      // 左下
    const auto p1 = d_point{xmin, ymax, 0, pnum + 1};  // 左上
    const auto p2 = d_point{xmax, ymin, 0, pnum + 2};  // 右下
    const auto p3 = d_point{xmax, ymax, 0, pnum + 3};  // 右上
    std::vector<d_point> n_points = points;
    n_points.push_back(p0);
    n_points.push_back(p1);
    n_points.push_back(p2);
    n_points.push_back(p3);

    // 左上 右下 以及其中一条对角线构建成的2个初始三角形
    d.triangles.emplace_back(d_triangle{p0, p1, p2});
    d.triangles.emplace_back(d_triangle{p3, p1, p2});
    // int iii = 0;
    for (auto const& pt : points)
    {
        // std::vector<d_edge> edges;
        std::map<int, std::map<int, int>> bad_edge_records;
        std::vector<d_triangle> tmps;
        int edge_num = 0;
        // 当前点在现有三角形外接圆的位置关系
        for (auto const& tri : d.triangles)
        {
            /* Check if the point is inside the triangle circum circle. */
            const double dist = (tri.circle.x - pt.x) * (tri.circle.x - pt.x) + (tri.circle.y - pt.y) * (tri.circle.y - pt.y);
            if (dist < tri.circle.radius)
            {  // 四点共圆, 不符合准则
                bad_edge_records[tri.e0.i0][tri.e0.i1]++;
                bad_edge_records[tri.e1.i0][tri.e1.i1]++;
                bad_edge_records[tri.e2.i0][tri.e2.i1]++;
                //// p1 p2和 p2 p1 是同一个边
                // edge_records[tri.e0.]
            }
            else
            {
                tmps.push_back(tri);
            }
        }
        int tri_num = tmps.size();

        int ignore_radius_cnt = 0;
        for (auto [i0, i1_c] : bad_edge_records)
        {
            for (auto [i1, c] : i1_c)
            {
                if (1 == c)
                {
                    edge_num++;
                    d_triangle ttt{n_points[i0], n_points[i1], {pt.x, pt.y, pt.v, pt.i}};
                    if (ttt.circle.radius == 0)  // 外接圆半径过小的就不处理了,可能点坐标就有问题
                    {
                        continue;
                        ignore_radius_cnt++;
                    }
                    tmps.push_back({ttt});
                }
            }
        }
        // std::cout << pt.i << "," << tri_num << "," << edge_num << std::endl;
        d.triangles = tmps;
    }
    return false;
}

bool silly_delaunay::triangulate()
{
    return false;
}
