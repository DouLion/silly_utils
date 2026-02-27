https://dsa.cs.tsinghua.edu.cn/~deng/cg/project/ 收录了一些相关的算法说明
https://github.com/abellgithub/delaunator-cpp 现在使用的二维实现,原本是mapbox的delaunator功能



void suDelaunay::BuildSegmentsB(const double& threshold, std::vector<RawSegment>& segments, std::unordered_map<EdgeID, size_t, EdgeID::Hash>& pending_map) const
{
    segments.clear();
    segments.reserve(m_tris.size() * 2);  // 预估大小，避免多次扩容
    pending_map.clear();

    const double th = threshold - 0.000001;  // 容差处理
    const double EPS = 1e-9;

    // 临时变量，避免循环内重复分配
    struct Cut
    {
        suPoint p;
        EdgeID e;
    };
    Cut cuts[2];

    // 遍历所有三角形
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

        if (cnt != 2)
            continue;  // 防御性编程，理论上不会发生

        // 3. 创建新线段
        size_t seg_idx = segments.size();
        segments.emplace_back();
        RawSegment& seg = segments.back();

        // 初始化线段数据
        // 约定：cuts[0] 对应 p[0]/e[0], cuts[1] 对应 p[1]/e[1]
        seg.p[0] = cuts[0].p;
        seg.e[0] = cuts[0].e;
        seg.p[1] = cuts[1].p;
        seg.e[1] = cuts[1].e;
        seg.next_idx = SIZE_MAX;
        seg.next_point_id = -1;
        seg.has_prev = false;

        // 4. 尝试配对两条边 (核心逻辑)
        // 我们需要对 seg 的两条边 e[0] 和 e[1] 分别尝试在 pending_map 中查找
        for (int side = 0; side < 2; ++side)
        {
            const EdgeID& current_edge = seg.e[side];

            auto it = pending_map.find(current_edge);
            if (it != pending_map.end())
            {
                // 【找到邻居！】
                size_t prev_idx = it->second;
                RawSegment& prev = segments[prev_idx];

                // 建立连接：prev -> seg
                // 此时 prev 还没有 next (因为如果有 next，它早就从 map 移除了)
                if (prev.next_idx == SIZE_MAX)
                {
                    prev.next_idx = seg_idx;
                    prev.has_prev = false;  // prev 可能是头，或者它的前驱还没处理，这里主要标记 seg 有前驱

                    // 【关键：预计算方向】
                    // prev 线段是通过 current_edge 连过来的。
                    // prev 的哪个端点在 current_edge 上？
                    // 如果 prev.e[0] == current_edge，说明 prev.p[0] 在桥上。
                    // 那么路径流向应该是：... -> prev.p[1] -> prev.p[0](桥) -> seg.p[side](桥) -> seg.p[1-side]
                    // 所以 prev 的下一个点应该是 seg 的 "1-side" 那个点。

                    prev.next_point_id = 1 - side;

                    // 标记 seg 有前驱 (用于后续找头)
                    seg.has_prev = true;
                }

                // 配对成功，从 Map 移除该边 (因为它已经闭合了，不再需要等待)
                pending_map.erase(it);
            }
            else
            {
                // 【没找到邻居】
                // 将当前线段注册到 Map，等待未来的三角形来配对这条边
                // 注意：如果同一个边被第二次插入（逻辑错误），这里会覆盖，但正常拓扑不会发生
                pending_map[current_edge] = seg_idx;
            }
        }
    }
}
