//
// Created by dell on 2024/1/24.
//

#include "silly_delaunay.h"
static constexpr double DELAUNAY_TRI_EPSILON = 1e-10;

static suPoint InterpPoint(const suPoint& p1, const suPoint& p2, double th)
{
    double t = (th - p1.v) / (p2.v - p1.v);

    // 按照比例 t 插值 x, y
    return suPoint(p1.x + (p2.x - p1.x) * t,
                   p1.y + (p2.y - p1.y) * t,
                   0,  // p1.z + (p2.z - p1.z) * t  如果有高程，也插值
                   th  // 切点的值必然是 th
    );
}

void suDelaunay::BuildTriangles(const std::vector<suPoint>& input_points)
{
    // 拷贝
    if (input_points.size() < 3)
    {
        return;
    }

    m_points = input_points;

    // 排序 (为了去重，也为了让三角剖分更稳定)
    std::sort(m_points.begin(), m_points.end(), [](const suPoint& a, const suPoint& b) {
        if (std::abs(a.x - b.x) > DELAUNAY_TRI_EPSILON)
            return a.x < b.x;
        return a.y < b.y;
    });

    // 去重 (std::unique)
    auto _fn_uinque = [](const suPoint& a, const suPoint& b) { return std::abs(a.x - b.x) < DELAUNAY_TRI_EPSILON && std::abs(a.y - b.y) < DELAUNAY_TRI_EPSILON; };
    auto last = std::unique(m_points.begin(), m_points.end(), _fn_uinque);
    m_points.erase(last, m_points.end());
    size_t num = m_points.size();
    suRect rect;

    for (const auto& p : m_points)
    {
        rect.min.x = std::min(rect.min.x, p.x);
        rect.min.y = std::min(rect.min.y, p.y);
        rect.max.x = std::max(rect.max.x, p.x);
        rect.max.y = std::max(rect.max.y, p.y);
    }

    {  // 最外边添加四个点
        suRect ex = rect.expand(0.1);
        std::vector<suPoint> expts = {{ex.min.x, ex.min.y, 0, 0}, {ex.max.x, ex.min.y, 0, 0}, {ex.max.x, ex.max.y, 0, 0}, {ex.min.x, ex.max.y, 0, 0}};
        m_points.insert(m_points.end(), expts.begin(), expts.end());
    }
    // 再排个序
    /*std::sort(m_points.begin(), m_points.end(), [](const suPoint& a, const suPoint& b) {
        if (std::abs(a.x - b.x) > DELAUNAY_TRI_EPSILON)
            return a.x < b.x;
        return a.y < b.y;
    });*/
    // 构建delaunator::Delaunator需要的顶点
    std::vector<double> coords;
    coords.reserve(m_points.size() * 2 + 8);
    const double scale = 1e6;
    for (const auto& p : m_points)
    {
        coords.push_back(p.x * scale);
        coords.push_back(p.y * scale);
    }
    delaunator::Delaunator d(coords);

    size_t triNum = d.triangles.size() / 3;
    m_tris.reserve(triNum);
    num -= 2;
    for (size_t i = 0; i < triNum; ++i)
    {
        // 注意：delaunator 返回的是点的索引
        size_t a = d.triangles[i * 3], b = d.triangles[i * 3 + 1], c = d.triangles[i * 3 + 2];

        /*if (a > num || b > num || c > num)
        {
            continue;
        }*/
        m_tris.push_back({d.triangles[i * 3], d.triangles[i * 3 + 1], d.triangles[i * 3 + 2]});
    }
}

void suDelaunay::TraceLine(const double& th, std::vector<suLine>& lines) const
{
    lines.clear();
    const auto& pts = m_points;
    const auto& tris = m_tris;
    // 邻接表: 记录某条边被哪些线段(index)连接
    // 正常情况下，内部边对应的 vector size 应该是 2，边界边是 1
    std::vector<RawSegment> raw_segs;
    std::unordered_map<EdgeID, std::vector<size_t>, EdgeID::Hash> adj_map;

    BuildSegments(th, raw_segs, adj_map);

    if (raw_segs.empty())
        return;
    // TODO: 线和面追踪逻辑应该也是重合的,面多了一步内外环关系的构建
}
void suDelaunay::TracePoly(const double& threshold, std::vector<suPoly>& polys) const
{
    polys.clear();
    const auto& pts = m_points;

    std::vector<RawSegment> segments;
    std::unordered_map<EdgeID, std::vector<size_t>, EdgeID::Hash> adj_map;
    // 降雨值一般保留小数点后1位
    // 使用的阈值一般也是小数点后1位
    // 减去一个较小值留有容差,简化判断,避免除0
    const double th = threshold - 0.000001;
    BuildSegments(th, segments, adj_map);

    if (segments.empty())
    {
        return;
    }

    std::vector<PolyNode> outers;
    std::vector<PolyNode> holes;

    for (size_t i = 0; i < segments.size(); ++i)
    {
        if (segments[i].visited)
        {
            continue;
        }
        PolyNode node;
        // 初始化环
        node.ring.points.push_back(segments[i].p1);
        node.ring.points.push_back(segments[i].p2);
        segments[i].visited = true;

        EdgeID tail = segments[i].e2;  // tail 用于存储当前计算到那个边
        bool extended = true;

        // 贪心追踪
        while (extended)
        {
            extended = false;
            const auto& neighbors = adj_map[tail];
            for (size_t idx : neighbors)
            {
                if (segments[idx].visited)
                {
                    continue;
                }

                RawSegment& nxtSeg = segments[idx];
                nxtSeg.visited = true;

                if (nxtSeg.e1 == tail)
                {
                    node.ring.points.push_back(nxtSeg.p2);
                    tail = nxtSeg.e2;
                }
                else
                {
                    node.ring.points.push_back(nxtSeg.p1);
                    tail = nxtSeg.e1;
                }
                extended = true;
            }
        }
        const suPoint& front = node.ring.points.front();
        const suPoint& back = node.ring.points.back();
        const suPoint& diff = front - back;
        if (node.ring.points.size() < 3)
        {
            std::cout << "不完整的环" << std::endl;
            continue;
        }
        if (node.ring.points.front().dist2(node.ring.points.back()) > 1e-9)
        {
            std::cout << "不闭合" << std::endl;
            continue;
        }

        // 计算几何属性
        bool greater = segments[i].p1.v > th;
        bool in = node.ring.contains(segments[i].p1);
        if (greater == in)
        {
            node.ring.is_outer = 1;  // 该环是外环
        }
        else
        {
            node.ring.is_outer = 0;  //  该环是内环环
        }

        node.bbox = node.ring.bound();
        node.area_abs = std::abs(node.ring.area());
        if (node.ring.is_outer)
        {
            outers.push_back(std::move(node));
        }
        else
        {
            holes.push_back(std::move(node));
        }
    }
#ifndef NDEBUG
    int notTraceSeg = 0;

    for (const auto& seg : segments)
    {
        if (!seg.visited)
        {
            notTraceSeg++;
        }
    }
    std::cout << "未追踪线段: " << notTraceSeg << std::endl;
#endif
    // 从小到大排序,保证内环在找父级时优先找到父级而不会找到爷爷级的
    std::sort(outers.begin(), outers.end(), [](const PolyNode& a, const PolyNode& b) { return a.area_abs < b.area_abs; });
    polys.resize(outers.size());
    for (int i = 0; i < polys.size(); ++i)
    {
        polys[i].outer.points.swap(outers[i].ring.points);
    }

    for (int j = 0; j < holes.size(); ++j)
    {
        for (int i = 0; i < polys.size(); ++i)
        {
            if (holes[j].area_abs >= outers[i].area_abs)
            {
                continue;
            }
            if (holes[j].ring.points.empty())  // 多余的
            {
                continue;
            }

            const suPoint& fp = holes[j].ring.points.front();
            if (outers[i].bbox.contains(fp))
            {
                if (polys[i].outer.contains(fp))
                {
                    polys[i].holes.push_back(std::move(holes[j].ring));
                    break;
                }
            }
        }
    }
#ifndef NDEBUG
    // 额外检查
    for (int j = 0; j < holes.size(); ++j)
    {
        if (holes[j].ring.points.empty())
        {
            std::cout << "孤儿内环" << std::endl;
        }
    }
#endif
}

void suDelaunay::BuildSegments(const double& threshold, std::vector<RawSegment>& segments, std::unordered_map<EdgeID, std::vector<size_t>, EdgeID::Hash>& out_adj) const
{
    segments.clear();
    out_adj.clear();

    const double th = threshold;

    for (const auto& [i1, i2, i3] : m_tris)
    {
        size_t idx[3] = {i1, i2, i3};
        const suPoint* p[3] = {&m_points[i1], &m_points[i2], &m_points[i3]};

        // 使用调整后的 th，只用 > 即可
        bool bigger[3] = {p[0]->v > th, p[1]->v > th, p[2]->v > th};

        if (bigger[0] == bigger[1] && bigger[1] == bigger[2])
        {
            // 全部大于阈值或者全部小于阈值
            // 那么等值线不会从该三角形中穿过
            continue;
        }
        struct Cut
        {
            suPoint p;
            EdgeID e;
        };
        Cut cuts[2];
        int cnt = 0;

        // 遍历三个边, 通过值判断
        for (int i = 0; i < 3; ++i)
        {
            int j = (i + 1) % 3;
            if (bigger[i] != bigger[j])  // 一个大一个小
            {
                if (cnt < 2)
                {
                    cuts[cnt].p = InterpPoint(*p[i], *p[j], th);  // 插值
                    cuts[cnt].e = EdgeID(idx[i], idx[j]);
                    cnt++;
                }
            }
        }

        size_t seg_idx = segments.size();
        segments.push_back({cuts[0].p, cuts[1].p, cuts[0].e, cuts[1].e});
        out_adj[cuts[0].e].push_back(seg_idx);
        out_adj[cuts[1].e].push_back(seg_idx);
    }
}
