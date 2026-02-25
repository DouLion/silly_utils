//
// Created by dell on 2024/1/24.
//

#include "silly_delaunay.h"
#include <datetime/silly_timer.h>
#include <log/silly_log.h>
static constexpr double DELAUNAY_TRI_EPSILON = 1e-6;

// 唯一的边 ID

static suPoint _InterpPoint(const suPoint& p1, const suPoint& p2, double th)
{
    // 防止分母为0 (虽然逻辑上跨越阈值时v不应该相等，但为了健壮性)
    if (std::abs(p2.v - p1.v) < 1e-9)
        return p1;

    double t = (th - p1.v) / (p2.v - p1.v);

    // 按照比例 t 插值 x, y
    // z 和 v 也可以插值，或者 v 直接设为 th
    return suPoint(p1.x + (p2.x - p1.x) * t,
                   p1.y + (p2.y - p1.y) * t,
                   p1.z + (p2.z - p1.z) * t,  // 如果有高程，也插值
                   th                         // 切点的值必然是 th
    );
}

void suDelaunay::BuildTri(const std::vector<suPoint>& input_points)
{
    // 1. 拷贝
    m_points = input_points;

    // 2. 排序 (为了去重，也为了让三角剖分更稳定)
    std::sort(m_points.begin(), m_points.end(), [](const suPoint& a, const suPoint& b) {
        if (std::abs(a.x - b.x) > DELAUNAY_TRI_EPSILON)
            return a.x < b.x;
        return a.y < b.y;
    });

    // 3. 去重 (std::unique)
    auto last = std::unique(m_points.begin(), m_points.end(), [](const suPoint& a, const suPoint& b) { return std::abs(a.x - b.x) < DELAUNAY_TRI_EPSILON && std::abs(a.y - b.y) < DELAUNAY_TRI_EPSILON; });
    m_points.erase(last, m_points.end());
    std::vector<double> coords;
    coords.reserve(m_points.size() * 2 + 8);
    suRect rect;
    for (const auto& p : m_points)
    {
        rect.min.x = std::min(rect.min.x, p.x);
        rect.min.y = std::min(rect.min.y, p.y);
        rect.max.x = std::max(rect.max.x, p.x);
        rect.max.y = std::max(rect.max.y, p.y);
        coords.push_back(p.x);
        coords.push_back(p.y);
    }

    {  // 最外边添加四个点
        suRect ex = rect.expand(0.1);
        std::vector<suPoint> expts = {{ex.min.x, ex.min.y}, {ex.max.x, ex.min.y}, {ex.max.x, ex.max.y}, {ex.min.x, ex.max.y}};
        for (const auto& p : expts)
        {
            coords.push_back(p.x);
            coords.push_back(p.y);
        }
        m_points.insert(m_points.end(), expts.begin(), expts.end());
    }
    delaunator::Delaunator d(coords);

    size_t triNum = d.triangles.size() / 3;
    m_tris.resize(triNum);

    for (size_t i = 0; i < triNum; ++i)
    {
        // 注意：delaunator 返回的是点的索引
        m_tris[i] = {d.triangles[i * 3], d.triangles[i * 3 + 1], d.triangles[i * 3 + 2]};
    }
}

void suDelaunay::TraceLine(const double& th, std::vector<suLine>& outPolylines) const
{
    outPolylines.clear();
    const auto& pts = m_points;
    const auto& tris = m_tris;

    std::vector<RawSegment> raw_segs;
    // 邻接表: 记录某条边被哪些线段(index)连接
    // 正常情况下，内部边对应的 vector size 应该是 2，边界边是 1
    std::unordered_map<EdgeID, std::vector<size_t>, EdgeID::Hash> adj_map;

    // === Step 1: 遍历三角形，提取所有切片 ===
    for (const auto& tri : tris)
    {
        size_t idx[3] = {std::get<0>(tri), std::get<1>(tri), std::get<2>(tri)};
        const suPoint* p[3] = {&pts[idx[0]], &pts[idx[1]], &pts[idx[2]]};
        bool b[3] = {p[0]->v >= th, p[1]->v >= th, p[2]->v >= th};

        // 快速剔除全在同侧的情况
        if (b[0] == b[1] && b[1] == b[2])
            continue;

        // 寻找交点
        struct Cut
        {
            suPoint p;
            EdgeID e;
        };
        std::vector<Cut> cuts;

        for (int i = 0; i < 3; ++i)
        {
            int j = (i + 1) % 3;
            if (b[i] != b[j])
            {
                // _InterpPoint 是你的插值函数
                cuts.push_back({_InterpPoint(*p[i], *p[j], th), EdgeID(idx[i], idx[j])});
            }
        }

        if (cuts.size() == 2)
        {
            size_t seg_idx = raw_segs.size();
            raw_segs.push_back({cuts[0].p, cuts[1].p, cuts[0].e, cuts[1].e});

            // 注册到邻接表
            adj_map[cuts[0].e].push_back(seg_idx);
            adj_map[cuts[1].e].push_back(seg_idx);
        }
    }

    if (raw_segs.empty())
        return;

    // === Step 2: 定义单向追踪函数 ===
    // 这里的逻辑是：给定一个起始线段和起始方向（进入的边），一直跑到底
    auto trace_path = [&](size_t start_idx, EdgeID start_edge) -> suLine {
        suLine poly;
        size_t curr_idx = start_idx;
        EdgeID curr_edge = start_edge;  // 当前我们要去寻找邻居的那个接口边

        // 先把起始线段放入 (需要判断方向，确保 curr_edge 是尾部)
        RawSegment& start_seg = raw_segs[curr_idx];
        start_seg.visited = true;

        if (start_seg.e1 == curr_edge)
        {
            // e1 是我们要连接的方向，说明 p2 是头，p1 是尾
            poly.push_back(start_seg.p2);
            poly.push_back(start_seg.p1);
        }
        else
        {
            // e2 是尾
            poly.push_back(start_seg.p1);
            poly.push_back(start_seg.p2);
        }

        // 开始循环延伸
        bool extended = true;
        while (extended)
        {
            extended = false;
            const auto& neighbors = adj_map[curr_edge];

            for (size_t next_idx : neighbors)
            {
                if (raw_segs[next_idx].visited)
                    continue;

                RawSegment& next_seg = raw_segs[next_idx];
                next_seg.visited = true;

                // 找到邻居了，接上去
                if (next_seg.e1 == curr_edge)
                {
                    // 邻居的 e1 连着我的尾巴，邻居的 p2 是新的尾巴
                    poly.push_back(next_seg.p2);
                    curr_edge = next_seg.e2;
                }
                else
                {
                    // 邻居的 e2 连着我的尾巴，邻居的 p1 是新的尾巴
                    poly.push_back(next_seg.p1);
                    curr_edge = next_seg.e1;
                }

                extended = true;
                break;  // 继续下一轮 while
            }
        }
        return poly;
    };

    // === Step 3: 优先处理“线头” (Open Polylines) ===
    // 线头特征：它的某一个端点边，在 adj_map 里只有 1 个引用

    for (size_t i = 0; i < raw_segs.size(); ++i)
    {
        if (raw_segs[i].visited)
            continue;

        int count1 = adj_map[raw_segs[i].e1].size();
        int count2 = adj_map[raw_segs[i].e2].size();

        // 只有当它是端点时才开始！
        // 如果 count1==1, 说明 e1 是边界，我们要往 e2 方向跑
        if (count1 == 1 && count2 != 1)
        {
            outPolylines.push_back(trace_path(i, raw_segs[i].e2));
        }
        // 如果 count2==1, 说明 e2 是边界，我们要往 e1 方向跑
        else if (count2 == 1 && count1 != 1)
        {
            outPolylines.push_back(trace_path(i, raw_segs[i].e1));
        }
        // 如果两头都是1，说明这根线就这一个片段（极短线），两头都不连
        else if (count1 == 1 && count2 == 1)
        {
            raw_segs[i].visited = true;
            suLine tiny;
            tiny.push_back(raw_segs[i].p1);
            tiny.push_back(raw_segs[i].p2);
            outPolylines.push_back(tiny);
        }
    }

    // === Step 4: 处理闭合环 (Closed Loops) ===
    // 剩下的未访问线段，一定都是闭合环的一部分（因为如果有头，Step 3 早就处理了）
    // 对于闭合环，从哪里开始都可以，只要一直单向跑，最后一定会连回来

    for (size_t i = 0; i < raw_segs.size(); ++i)
    {
        if (raw_segs[i].visited)
            continue;

        // 随便选一个方向跑 (比如往 e2 跑)
        // 跑完的结果首尾应该是非常接近的（在同一个三角形边上）
        suLine loop = trace_path(i, raw_segs[i].e2);

        outPolylines.push_back(loop);
    }
}
void suDelaunay::TracePoly(const double& input_th, std::vector<suPoly>& polys) const
{
    polys.clear();
    const auto& pts = m_points;
    const auto& tris = m_tris;

    // === 1. 阈值微调 (Epsilon Shift) ===
    // 使得 "等于" 变为 "大于", 避免除零和重合判断
    double th = input_th - 0.0001;

    // === 2. 构建线段 (Build Segments) ===
    // 这一步和 TraceLine 是一样的，只是为了代码独立完整写在这里
    struct RawSegment
    {
        suPoint p1, p2;
        EdgeID e1, e2;
        bool visited = false;
    };
    std::vector<RawSegment> raw_segs;
    std::unordered_map<EdgeID, std::vector<size_t>, EdgeID::Hash> adj_map;

    for (const auto& tri : tris)
    {
        size_t idx[3] = {std::get<0>(tri), std::get<1>(tri), std::get<2>(tri)};
        const suPoint* p[3] = {&pts[idx[0]], &pts[idx[1]], &pts[idx[2]]};

        // 使用调整后的 th，只用 > 即可
        bool b[3] = {p[0]->v > th, p[1]->v > th, p[2]->v > th};

        if (b[0] == b[1] && b[1] == b[2])
            continue;

        struct Cut
        {
            suPoint p;
            EdgeID e;
        };
        Cut cuts[2];
        int cnt = 0;

        for (int i = 0; i < 3; ++i)
        {
            int j = (i + 1) % 3;
            if (b[i] != b[j])
            {
                if (cnt < 2)
                {
                    cuts[cnt].p = _InterpPoint(*p[i], *p[j], th);  // 插值
                    cuts[cnt].e = EdgeID(idx[i], idx[j]);
                    cnt++;
                }
            }
        }

        if (cnt == 2)
        {
            size_t seg_idx = raw_segs.size();
            raw_segs.push_back({cuts[0].p, cuts[1].p, cuts[0].e, cuts[1].e});
            adj_map[cuts[0].e].push_back(seg_idx);
            adj_map[cuts[1].e].push_back(seg_idx);
        }
    }

    if (raw_segs.empty())
        return;

    // === 3. 追踪闭合环 (Trace Rings) ===
    // 此时不关心方向，不关心极性，只管把线连成圈
    struct PolyNode
    {
        suRing ring;
        suRect bbox;          // 缓存包围盒，加速包含检测
        double area_abs;      // 面积绝对值，用于排序
        int parent_idx = -1;  // 在 nodes 数组中的父级索引
        int depth = 0;        // 嵌套深度
    };
    std::vector<PolyNode> nodes;

    for (size_t i = 0; i < raw_segs.size(); ++i)
    {
        if (raw_segs[i].visited)
            continue;

        PolyNode node;
        // 初始化环
        node.ring.points.push_back(raw_segs[i].p1);
        node.ring.points.push_back(raw_segs[i].p2);
        raw_segs[i].visited = true;

        EdgeID curr_edge_tail = raw_segs[i].e2;
        bool extended = true;

        // 贪心追踪
        while (extended)
        {
            extended = false;
            const auto& neighbors = adj_map[curr_edge_tail];
            for (size_t next_idx : neighbors)
            {
                if (raw_segs[next_idx].visited)
                    continue;

                RawSegment& next_seg = raw_segs[next_idx];
                next_seg.visited = true;

                if (next_seg.e1 == curr_edge_tail)
                {
                    node.ring.points.push_back(next_seg.p2);
                    curr_edge_tail = next_seg.e2;
                }
                else
                {
                    node.ring.points.push_back(next_seg.p1);
                    curr_edge_tail = next_seg.e1;
                }
                extended = true;
                break;
            }
        }

        // 闭合检查 (去除未闭合的断线)
        if (node.ring.points.size() < 3 || node.ring.points.front().dist2(node.ring.points.back()) > 1e-9)
        {
            continue;
        }
        node.ring.points.pop_back();  // 移除重复的首尾点

        // 计算几何属性
        node.bbox = node.ring.bound();               // 你的 suRing 应该有 bound()
        node.area_abs = std::abs(node.ring.area());  // 你的 suRing 有 area()

        nodes.push_back(std::move(node));
    }

    // === 4. 深度排序与层级构建 (Depth Sorting) ===
    // 核心逻辑：大的包围小的。

    // 4.1 按面积从大到小排序 (Desc)
    std::sort(nodes.begin(), nodes.end(), [](const PolyNode& a, const PolyNode& b) { return a.area_abs > b.area_abs; });

    // 4.2 确定父子关系
    // O(N^2) 复杂度，配合 AABB Check 效率很高
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        int best_parent = -1;

        // 倒序查找：因为我们按面积降序排列，i 的父级一定在 [0, i-1] 之间
        // 倒序找，找到的第一个包含 i 的必定是"最小包围矩形" (直接父级)
        for (int j = (int)i - 1; j >= 0; --j)
        {
            // AABB 快速过滤
            // 假设 suRect 这里的 contains 是判断矩形包含矩形
            if (!nodes[j].bbox.intersect(nodes[i].bbox))
                continue;

            // 精确判断：点在面内
            // 只要有一个点在里面，整个环就在里面 (因为等值线互不相交)
            if (nodes[j].ring.intersect(nodes[i].ring.points[0]))
            {
                best_parent = j;
                break;
            }
        }

        if (best_parent != -1)
        {
            nodes[i].parent_idx = best_parent;
            nodes[i].depth = nodes[best_parent].depth + 1;
        }
        else
        {
            nodes[i].parent_idx = -1;
            nodes[i].depth = 0;  // 顶层
        }
    }

    // === 5. 输出转换 (Map to suPoly) ===
    // 规则：偶数层(0,2,4)为实体(Outer)，奇数层(1,3,5)为孔洞(Hole)

    // 建立映射：nodes下标 -> polys结果集下标
    std::vector<int> node_to_poly_map(nodes.size(), -1);

    for (size_t i = 0; i < nodes.size(); ++i)
    {
        // 偶数层级 -> 是一个新的 Outer
        if (nodes[i].depth % 2 == 0)
        {
            suPoly p;
            p.outer = std::move(nodes[i].ring);
            p.outer.is_outer = true;  // 标记属性

            polys.push_back(std::move(p));
            node_to_poly_map[i] = (int)polys.size() - 1;
        }
        // 奇数层级 -> 是上一层 Outer 的 Hole
        else
        {
            int parent_node_idx = nodes[i].parent_idx;

            // 理论上 parent 必须存在且是偶数层 (Outer)
            if (parent_node_idx != -1 && node_to_poly_map[parent_node_idx] != -1)
            {
                int target_poly_idx = node_to_poly_map[parent_node_idx];
                nodes[i].ring.is_outer = false;
                polys[target_poly_idx].holes.push_back(std::move(nodes[i].ring));
            }
            else
            {
                // [孤儿保护]
                // 如果这是个奇数层，但找不到爸爸 (极罕见，可能因浮点误差边界导致)
                // 把它强转为 Outer，防止丢失
                std::cout << "orphan node, force to outer" << std::endl;
                suPoly p;
                p.outer = std::move(nodes[i].ring);
                p.outer.is_outer = true;
                polys.push_back(std::move(p));
                node_to_poly_map[i] = (int)polys.size() - 1;
            }
        }
    }
}

void suDelaunay::_BuildSegments(const double& th, std::vector<RawSegment>& out_segs, std::unordered_map<EdgeID, std::vector<size_t>, EdgeID::Hash>& out_adj) const
{
    out_segs.clear();
    out_adj.clear();

    const auto& pts = m_points;
    const auto& tris = m_tris;

    // 遍历三角形，提取切片
    for (const auto& tri : tris)
    {
        size_t idx[3] = {std::get<0>(tri), std::get<1>(tri), std::get<2>(tri)};
        const suPoint* p[3] = {&pts[idx[0]], &pts[idx[1]], &pts[idx[2]]};
        bool b[3] = {p[0]->v >= th, p[1]->v >= th, p[2]->v >= th};

        // 快速剔除全在同侧的情况
        if (b[0] == b[1] && b[1] == b[2])
            continue;

        // 寻找交点
        struct CutInfo
        {
            suPoint pt;
            EdgeID edge;
        };
        CutInfo cuts[2];
        int cut_count = 0;

        for (int i = 0; i < 3; ++i)
        {
            int j = (i + 1) % 3;
            if (b[i] != b[j])
            {
                if (cut_count < 2)
                {
                    cuts[cut_count].pt = _InterpPoint(*p[i], *p[j], th);
                    cuts[cut_count].edge = EdgeID(idx[i], idx[j]);
                    cut_count++;
                }
            }
        }

        if (cut_count == 2)
        {
            size_t seg_idx = out_segs.size();
            out_segs.push_back({cuts[0].pt, cuts[1].pt, cuts[0].edge, cuts[1].edge});

            // 构建邻接表
            out_adj[cuts[0].edge].push_back(seg_idx);
            out_adj[cuts[1].edge].push_back(seg_idx);
        }
    }
}

#ifndef NDEBUG
#include <graphics/render/canvas/silly_cairo.h>
#include <database/otl/silly_otl.h>
void suDelaunay::Test2()
{
    double width = 10000;
    double height = 10000;
    std::vector<suPoint> pts;
    int num = 20000;
    double th = 60.1;
    std::vector<double> vs = RANDOM(0.1 * width, 0.9 * width, num * 2);
    std::vector<double> dd = RANDOM(10, 120, num);
    suPoint p0(width / 2, height / 2);
    for (int i = 0; i < num; i++)
    {
        suPoint tmp(vs[i * 2], vs[i * 2 + 1]);
        // tmp.v = tmp.dist(p0);
        tmp.v = (int)dd[i];
        pts.push_back(tmp);
    }
    suTimer timer;
    BuildTri(pts);
    SLOG_DEBUG("点数量: {}, 时间: {} ms", m_points.size(), timer.elapsed_ms())
    SLOG_DEBUG("三角形数量: {}", m_tris.size());
    std::vector<suLine> lines;

    timer.restart();
    TraceLine(th, lines);
    SLOG_DEBUG("线数量: {}, 时间: {} ms", lines.size(), timer.elapsed_ms())

    std::vector<suPoly> polys;
    timer.restart();
    TracePoly(th, polys);
    SLOG_DEBUG("面数量: {}, 时间: {} ms", polys.size(), timer.elapsed_ms())

    suCairo cairo;
    cairo.create(width, height);
    cairo.set(suColor(102, 255, 102, 255));
    if (1)
    {
        for (const auto& poly : polys)
        {
            cairo.draw_poly(poly);
        }
    }
    else
    {
        suPoly nep;
        nep.outer.points = lines.front().to_vector();
        for (int i = 1; i < lines.size(); ++i)
        {
            suRing tmp;
            tmp.points = lines[i].to_vector();
            nep.holes.push_back(tmp);
        }
        cairo.draw_poly(nep);
    }
   
    if (0)
    {  // 三角形
        cairo.set(suColor(255, 51, 153, 255));
        for (const auto& [a, b, c] : m_tris)
        {
            cairo.draw_line({m_points[a], m_points[b], m_points[c], m_points[a]});
        }
    }

    cairo.set(suColor(51, 51, 255, 255));
    for (const auto& line : lines)
    {
        cairo.draw_line(line.to_vector());
    }

    for (const auto& p : m_points)
    {
        if (p.v >= th)
        {
            cairo.set(suColor(255, 0, 0, 255));  // 红
        }
        else
        {
            cairo.set(suColor(0, 0, 0, 255));  //  黑
        }
        cairo.draw_point(p, 6);
    }

    cairo.write("./all.png");
    cairo.release();
}
void suDelaunay::TestWithDB()
{
    std::string sql = "select b.LGTD, b.LTTD, SUM(DRP) as total from ST_PPTN_R a LEFT JOIN ST_STBPRP_B b on a.STCD = b.STCD where TM > '2024-06-24 08:00' and TM < '2024-06-25 08:00' and DRP > 0 GROUP BY a.STCD order  by total desc";
    std::string db = R"(
{
  "type": "mysql",
  "ip": "192.168.0.73",
  "port": 3306,
  "driver": "MariaDB ODBC 3.1 Driver",
  "schema": "RWDB_HN",
  "user": "root",
  "password": "3edc9ijn~"
}
)";

    std::vector<double> thresholds = {0.1, 10, 25, 50, 100, 150, 250};
    std::vector<std::string> colors = {"afc9ecc0", "afa9dd9b", "af7bcc7e", "af009989", "af003a8f", "af00006e", "af00004b"};
    suOTL otl;
    otl.from_json(db);
    otl.check();
    std::vector<suPoint> points;
    if (!otl.select(sql,
                    [&points](otl_stream* stm) {
                        while (!stm->eof())
                        {
                            std::string stcd;
                            double lgtd, lttd, total;
                            otl_read_row(*stm, lgtd, lttd, total);
                            points.push_back({lgtd, lttd, 0, total});
                        }
                    }))
    {
        SLOG_ERROR(otl.err())
        return;
    }
    suTimer timer;
    BuildTri(points);
    suRect bound;
    bound.min = {108.5, 24.5};
    bound.max = {114.5, 30.5};
    int width = 2000;
    int height = 2000;
    suCairo cairo;
    cairo.create(width, height);
    for (int i = 0; i < thresholds.size(); ++i)
    {
        double th = thresholds[i];
        suColor color;
        color.hex2argb(colors[i]);
        std::vector<suPoly> polys;
        TracePoly(th, polys);
        for (auto& poly : polys)
        {
            poly.outer.points = SU_SMOOTH_B_SPLINE(poly.outer.points, 10);
            for (auto& ring : poly.holes)
            {
                ring.points = SU_SMOOTH_B_SPLINE(ring.points, 10);
            }
        }
        cairo.set(color);
        for (const auto& poly : polys)
        {
            cairo.draw_poly(poly, bound);
        }
    }
    SLOG_INFO("{}ms", timer.elapsed_ms())
    cairo.write("./iso_db.png");
    cairo.release();
}

void suDelaunay::Test()
{
    double width = 2000;
    double height = 2000;
    std::vector<suPoint> pts;
    int num = 1000;
    std::vector<double> vs = RANDOM(0.1 * width, 0.9 * width, num * 2);
    std::vector<double> dd = RANDOM(10, 120, num);
    suPoint p0(width / 2, height / 2);
    for (int i = 0; i < num; i++)
    {
        suPoint tmp(vs[i * 2], vs[i * 2 + 1]);
        // tmp.v = tmp.dist(p0);
        tmp.v = (int)dd[i];
        pts.push_back(tmp);
    }
    suTimer timer;
    BuildTri(pts);
    SLOG_DEBUG("点数量: {}, 时间: {} ms", m_points.size(), timer.elapsed_ms())
    SLOG_DEBUG("三角形数量: {}", m_tris.size());

    double th = 60.1;
    // auto tris = delaunay.triangles();
    {  // 画三角
        suCairo cairo;
        cairo.create(width, height);
        cairo.set(suColor(0, 0, 255, 255));
        suRect nr;
        nr.min = suPoint(0, 0);
        nr.max = suPoint(width, height);
        for (const auto& [a, b, c] : m_tris)
        {
            cairo.draw_line({m_points[a], m_points[b], m_points[c], m_points[a]});
        }
        cairo.set(suColor(255, 0, 0, 255));
        for (const auto& p : m_points)
        {
            cairo.draw_point(p, 4);
        }
        cairo.write("./tri.png");
    }
    if (0)
    {
        std::vector<suLine> lines;

        timer.restart();
        TraceLine(th, lines);
        SLOG_DEBUG("线数量: {}, 时间: {} ms", lines.size(), timer.elapsed_ms())
        // 画线
        suCairo cairo;
        cairo.create(width, height);
        cairo.set(suColor(0, 0, 255, 255));

        for (const auto& line : lines)
        {
            cairo.draw_line(line.to_vector());
        }
        cairo.set(suColor(255, 0, 0, 255));
        for (const auto& p : m_points)
        {
            cairo.draw_point(p, 4);
        }
        cairo.write("./isoline.png");
    }

    if (1)
    {
        std::vector<suPoly> polys;
        timer.restart();
        TracePoly(th, polys);
        SLOG_DEBUG("面数量: {}, 时间: {} ms", polys.size(), timer.elapsed_ms())
        // 画线
        suCairo cairo;
        cairo.create(width, height);
        cairo.set(suColor(0, 0, 255, 255));

        for (const auto& poly : polys)
        {
            cairo.draw_poly(poly);
        }

        for (const auto& p : m_points)
        {
            if (p.v >= th)
            {
                cairo.set(suColor(255, 0, 0, 255));
            }
            else
            {
                cairo.set(suColor(0, 255, 0, 255));
            }
            cairo.draw_point(p, 6);
        }
        cairo.write("./isoarea.png");
    }
}
#endif