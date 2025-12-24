/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/24 11:53
 * @version: 1.0.1
 * @description: 德劳内 Bowyer-Watson algorithm C++ implementation of
 * http://paulbourke.net/papers/triangulate .
 * https://dsa.cs.tsinghua.edu.cn/~deng/cg/project/2000f/2000f-e/sweep.htm
 * https://en.wikipedia.org/wiki/Fortune%27s_algorithm
 * https://github.com/JCash/voronoi/blob/dev/src/jc_voronoi.h
 * 扫描线算法
 */
#ifndef SILLY_DELAUNAY_H
#define SILLY_DELAUNAY_H

#include <geo/silly_geo.h>
#include <graphics/render/canvas/silly_cairo.h>

/// Bowyer–Watson 算法实现的德劳内三角
class DelaunayBW
{
public:
    class Tri
    {
    public:
        Tri() = default;
        Tri(const suPoint& a1, const suPoint& b1, const suPoint& c1);
        /// 计算外接圆
        void CalcCircum();

        /// 点是否在外接圆内
        bool InCircum(const suPoint& p) const;
        suPoint CP;
        double R = 0;
        double R2 = 0;
        // 三个顶点
        suPoint a;
        suPoint b;
        suPoint c;
    };
    struct Edge {
        suPoint p1, p2;

        Edge(const suPoint& a, const suPoint& b) {
            if (a < b) { p1 = a; p2 = b; }
            else       { p1 = b; p2 = a; }
        }

        bool operator==(const Edge& other) const {
            return p1 == other.p1 && p2 == other.p2;
        }

        struct Hash {
            size_t operator()(const Edge& e) const {
                return std::hash<double>()(e.p1.x) ^ std::hash<double>()(e.p1.y)
                     ^ std::hash<double>()(e.p2.x) ^ std::hash<double>()(e.p2.y);
                // 更健壮的哈希可使用 boost::hash 或组合策略
            }
        };
    };
  public:
    void points(std::set<suPoint>& pts);
    void triangulate();
    void draw(const suPath& file);

    std::vector<Tri> triangles()
    {
        return m_tris;
    }

    void test();
protected:
    bool isSuperVertex(const suPoint& p) const {
        return m_superVertices.find(p) != m_superVertices.end();
    }

    void removeSuperTriangles() {
        std::vector<Tri> filtered;
        for (auto& tri : m_tris) {
            if (!isSuperVertex(tri.a) &&
                !isSuperVertex(tri.b) &&
                !isSuperVertex(tri.c)) {
                filtered.push_back(std::move(tri));
                }
        }
        m_tris = std::move(filtered);
    }

  protected:
    std::set<suPoint> m_points;
    std::vector<Tri> m_tris;
    suRect m_rect;
    std::set<suPoint> m_superVertices;
};

#endif  // SILLY_DELAUNAY_H
