//
// Created by dell on 2024/1/24.
//

#include "silly_delaunay.h"
#include <datetime/silly_timer.h>
#include <log/silly_log.h>
DelaunayBW::Tri::Tri(const suPoint& a1, const suPoint& b1, const suPoint& c1)
{
    a = a1;
    b = b1;
    c = c1;
    CalcCircum();
}
void DelaunayBW::Tri::CalcCircum()
{
    double A = a.x * a.x + a.y * a.y;
    double B = b.x * b.x + b.y * b.y;
    double C = c.x * c.x + c.y * c.y;
    double D = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

    const double EPS = 1e-12;
    if (std::abs(D) < EPS) {
        // 退化三角形：设为极大圆
#ifndef NDEBUG
        std::cout << "HUGE" << std::endl;
#endif
        CP = suPoint(0, 0);
        R2 = std::numeric_limits<double>::max();
        return;
    }

    CP.x = (a.x * (B - C) + b.x * (C - A) + c.x * (A - B)) / (2 * D);
    CP.y = (a.y * (B - C) + b.y * (C - A) + c.y * (A - B)) / (2 * D);
    R2 = (CP.x - a.x) * (CP.x - a.x) + (CP.y - a.y) * (CP.y - a.y);

}
bool DelaunayBW::Tri::InCircum(const suPoint& p) const
{
    suPoint np = p - CP;
    return np.x * np.x + np.y * np.y <= R2;
}
void DelaunayBW::points(std::set<suPoint>& pts)
{
    m_points = pts;
    m_tris.clear();
    m_superVertices.clear();

    if (pts.empty()) return;

    // 计算包围盒
    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::lowest();

    for (const auto& p : pts) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }

    double delta = std::max(max_x - min_x, max_y - min_y) * 10.0;
    if (delta == 0) delta = 10.0; // 所有点重合

    suPoint p1(min_x - delta, min_y - delta);
    suPoint p2(min_x - delta, max_y + delta);
    suPoint p3(max_x + delta, min_y - delta);

    m_superVertices = {p1, p2, p3};
    m_tris.emplace_back(p1, p2, p3);

}
void DelaunayBW::triangulate()
{
    for (const auto& pt : m_points) {
        // Step 1: 标记坏三角形
        std::vector<bool> toErase(m_tris.size(), false);
        std::vector<size_t> badIndices;

        for (size_t i = 0; i < m_tris.size(); ++i) {
            if (m_tris[i].InCircum(pt)) {
                toErase[i] = true;
                badIndices.push_back(i);
            }
        }

        // Step 2: 统计边出现次数
        std::unordered_map<Edge, int, Edge::Hash> edgeCount;
        for (size_t i : badIndices) {
            const auto& t = m_tris[i];
            edgeCount[Edge(t.a, t.b)]++;
            edgeCount[Edge(t.b, t.c)]++;
            edgeCount[Edge(t.c, t.a)]++;
        }

        // Step 3: 构建新三角形（仅边界边）
        std::vector<Tri> newTris;
        for (const auto& [edge, count] : edgeCount) {
            if (count == 1) {
                newTris.emplace_back(edge.p1, edge.p2, pt);
            }
        }

        // Step 4: 批量删除坏三角形
        std::vector<Tri> temp;
        for (size_t i = 0; i < m_tris.size(); ++i) {
            if (!toErase[i]) {
                temp.push_back(std::move(m_tris[i]));
            }
        }
        m_tris = std::move(temp);

        // Step 5: 添加新三角形
        m_tris.insert(m_tris.end(),
                      std::make_move_iterator(newTris.begin()),
                      std::make_move_iterator(newTris.end()));
    }

    // Step 6: 移除包含超三角形顶点的三角形
    removeSuperTriangles();
}
void DelaunayBW::draw(const suPath& file)
{
}
void DelaunayBW::test(){

    double width = 1000;
    double height = 1000;
    std::set<suPoint> pts;
    int num = 50;
    std::vector<double> vs = RANDOM(0.1* width, 0.9 * width, num * 2);
    for (int i = 0; i < num; i++)
    {
        pts.insert(suPoint(vs[i*2], vs[i*2+1]));
    }
    suTimer timer;
    points(pts);
    triangulate();
    SLOG_DEBUG("点数量: {}, 时间: {} ms", num, timer.elapsed_ms())
    SLOG_DEBUG("三角形数量: {}", m_tris.size());
    // auto tris = delaunay.triangles();
    suCairo cairo;
    cairo.create(width, height);
    cairo.set(suColor(0,0,255, 255));
    suRect nr;
    nr.min = suPoint(0,0);
    nr.max = suPoint(width,height);
    for (const auto& tri : m_tris)
    {
        cairo.draw_line({tri.a, tri.b, tri.c,tri.a}, nr);
    }
    cairo.set(suColor(255,0,0, 255));
    for (const auto& p : pts)
    {
        cairo.draw_point(p, 4);;
    }
    cairo.write("./tri.png");
}