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
#include <geo/triangular/delaunator.hpp>

class suDelaunay
{
public:
    void BuildTriangles(const std::vector<suPoint>& points);

    void TraceLine(const double& th, std::vector<suLine>& lines) const;
    void TracePoly(const double& th, std::vector<suPoly>& polys) const;
#ifndef NDEBUG
    void Test();
    void Test2();
    void TestWithDB();

#endif


  private:
    struct EdgeID
    {
        size_t u, v;
        EdgeID() : u(0), v(0)
        {
        }
        EdgeID(size_t a, size_t b)
        {
            if (a < b)
            {
                u = a;
                v = b;
            }
            else
            {
                u = b;
                v = a;
            }
        }
        bool operator==(const EdgeID& o) const
        {
            return u == o.u && v == o.v;
        }

        struct Hash
        {
            size_t operator()(const EdgeID& e) const
            {
                // 简单的移位异或哈希
                return std::hash<size_t>()(e.u) ^ (std::hash<size_t>()(e.v) << 1);
            }
        };
    };
    struct RawSegment // 穿过三角形的线段
    {
        suPoint p1, p2;
        EdgeID e1, e2;
        bool visited = false;
    };
     // 提取公共逻辑：构建线段片段
    void BuildSegments(const double& threshold, std::vector<RawSegment>& segments, std::unordered_map<EdgeID, std::vector<size_t>, EdgeID::Hash>& out_adj) const;
  protected:

    std::vector<suPoint> m_points;
    std::vector<std::tuple<size_t,size_t,size_t>> m_tris; // 存的是在m_points中的索引
};

#endif  // SILLY_DELAUNAY_H
