/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-03
 * @file: silly_delaunay_bowyer.cpp
 * @description: silly_delaunay_bowyer实现
 * @version: v1.0.1 2024-09-03 dou li yang
 */
#include "silly_delaunay_bowyer.h"
void silly_delaunay_bowyer::points(std::vector<silly_dt_point>& pts)
{
    m_points = pts;
}
void silly_delaunay_bowyer::triangulate()
{
    init_super_triangle();
    int step = 0;
    int ptidx = 0;
    for (auto const& pt : m_points)
    {
        // 每次加入一个新点时,之前的三角形都有可能被破坏
        std::vector<silly_dt_tri> check_tris = m_tris;
        m_tris.clear();
        std::set<silly_dt_edge> invalid_edges;
        for (auto& tri : check_tris)
        {
            tri.calc_circle();
            if (tri.in_circle(pt))
            {
                invalid_edges.insert(silly_dt_edge(tri.p0, tri.p1));
                invalid_edges.insert(silly_dt_edge(tri.p1, tri.p2));
                invalid_edges.insert(silly_dt_edge(tri.p2, tri.p0));
                /*invalid_edges.push_back(silly_dt_edge(tri.p0, tri.p1));
               invalid_edges.push_back(silly_dt_edge(tri.p1, tri.p2));
               invalid_edges.push_back(silly_dt_edge(tri.p2, tri.p0));*/
                /*   silly_dt_edge e1(tri.p0, tri.p1), e2(tri.p1, tri.p2), e3(tri.p2, tri.p0);
                   invalid_edges[e1.hash] = e1;
                   invalid_edges[e2.hash] = e2;
                   invalid_edges[e3.hash] = e3;*/
            }
            else
            {
                m_tris.emplace_back(tri);
            }
        }

        // 删除重合边, 就能形成空腔
        std::vector<bool> remove(invalid_edges.size(), false);
        for (auto it1 = invalid_edges.begin(); it1 != invalid_edges.end(); ++it1)
        {
            for (auto it2 = invalid_edges.begin(); it2 != invalid_edges.end(); ++it2)
            {
                if (it1 == it2)
                {
                    continue;
                }
                if (*it1 == *it2)
                {
                    remove[std::distance(invalid_edges.begin(), it1)] = true;
                    remove[std::distance(invalid_edges.begin(), it2)] = true;
                }
            }
        }
        // invalid_edges.erase(std::remove_if(invalid_edges.begin(), invalid_edges.end(), [&](auto const& e) { return remove[&e - &invalid_edges[0]]; }), invalid_edges.end());
        // 根据空腔和新点构建新的三角
        for (auto& edge : invalid_edges)
        {
            m_tris.emplace_back(silly_dt_tri(edge.p0, edge.p1, pt));
        }
        std::cout << ptidx++ << " \t " << invalid_edges.size() << std::endl;
        /* step++;
         draw("step_" + std::to_string(step)+".png");*/
    }

    // 略微收缩辅助窗口
    xmax -= ddx * 0.1;
    xmin += ddx * 0.1;
    ymax -= ddy * 0.1;
    ymin += ddy * 0.1;
    std::vector<silly_dt_tri> result;
    for (auto& tri : m_tris)
    {
        if (!contains_super_tri(tri))
        {
            result.emplace_back(tri);
        }
    }
    m_tris = result;
}
void silly_delaunay_bowyer::init_super_triangle()
{
    // 最大矩形
    for (auto p : m_points)
    {
        xmax = std::max(xmax, p.x);
        ymax = std::max(ymax, p.y);
        xmin = std::min(xmin, p.x);
        ymin = std::min(ymin, p.y);
    }
    ddx = xmax - xmin;
    ddy = ymax - ymin;
    xmax = xmax + ddx * 0.3;
    xmin = xmin - ddx * 0.3;
    ymax = ymax + ddy * 0.3;
    ymin = ymin - ddy * 0.3;

    m_tris.push_back(silly_dt_tri({xmin, ymin, UINT32_MAX - 1}, {xmax, ymin, UINT32_MAX - 2}, {xmax, ymax, UINT32_MAX - 3}));
    m_tris.push_back(silly_dt_tri({xmin, ymin, UINT32_MAX - 1}, {xmax, ymax, UINT32_MAX - 3}, {xmin, ymax, UINT32_MAX - 4}));
}
bool silly_delaunay_bowyer::contains_super_tri(const silly_dt_tri& tri) const
{
    if (tri.p0.x < xmin || tri.p0.x > xmax || tri.p0.y < ymin || tri.p0.y > ymax)
        return true;
    if (tri.p1.x < xmin || tri.p1.x > xmax || tri.p1.y < ymin || tri.p1.y > ymax)
        return true;
    if (tri.p2.x < xmin || tri.p2.x > xmax || tri.p2.y < ymin || tri.p2.y > ymax)
        return true;
    return false;
}
