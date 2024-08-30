//
// Created by dell on 2024/1/24. 单纯处理算法
//

#include "silly_iso_line.h"
#include <su_marco.h>
#include <triangular/silly_delaunay_bowyer.h>
#include <malloc.h>
#include <memory>
#include <thread>
#include <chrono>
#if _OPENMP
#include <omp.h>
#endif

silly_iso_line::silly_iso_line(const int& type)
{
    m_Type = static_cast<enum_iso_type>(type);
}

silly_iso_line::~silly_iso_line()
{
    clear();
}

void silly_iso_line::set_rect(const silly_geo_rect& rect)
{
    dMinLGTD = rect.left;
    dMaxLTTD = rect.top;
    dMaxLGTD = rect.right;
    dMinLTTD = rect.bottom;
}

void silly_iso_line::set_data(const size_t& num, const double* lgtd, const double* lttd, const double* data)
{
    m_data_num = num;  // +4;
    m_ddx = (double*)malloc(m_data_num * sizeof(double));
    m_ddy = (double*)malloc(m_data_num * sizeof(double));
    m_ddz = (double*)malloc(m_data_num * sizeof(double));
    double right = 0, top = 0;
    double left = 9999, bottom = 9999;
    double tx, ty, tz;
    for (int i = 0; i < num; ++i)
    {
        // TODO: 这里是不是有点问题
        tx = lgtd[i];
        ty = lttd[i];
        tz = data[i];
        right = std::max(right, tx);
        left = std::min(left, tx);
        top = std::max(top, ty);
        bottom = std::min(bottom, ty);
        m_max_dz = std::max(m_max_dz, tz);
        m_min_dz = std::min(m_min_dz, tz);
        m_ddx[i] = tx;
        m_ddy[i] = ty;
        m_ddz[i] = tz;
    }

    m_max_dz = ((int)(m_max_dz / 10.) + 1) * 10;  // 取得比maxz大的整数
    m_min_dz = ((int)(m_min_dz / 10.)) * 10;      // 取得比minz小的整数
    // 把所有数据变为正值
    if (m_min_dz < 0)
    {
        for (int i = 0; i < num; i++)
        {
            m_ddz[i] -= (m_min_dz);
        }
    }

    // 上下外扩一个面
    double exleft, exright, extop, exbottom;
    exleft = left - (right - left) / 10.;
    exright = right + (right - left) / 10.;
    extop = top + (top - bottom) / 10.;
    exbottom = bottom - (top - bottom) / 10.;

    // m_ddx[num] = exleft;
    // m_ddy[num] = extop;
    // m_ddz[num] = 0;

    // m_ddx[num] = exleft;
    // m_ddy[num] = bottom;
    // m_ddz[num] = 0;

    // m_ddx[num] = exright;
    // m_ddy[num] = extop;
    // m_ddz[num] = 0;

    // m_ddx[num] = exright;
    // m_ddy[num] = bottom;
    // m_ddz[num] = 0;
}

void silly_iso_line::set_level(const std::vector<double>& thresholds)
{
    m_thresholds = thresholds;
}
#define S11 1
void silly_iso_line::make_tri_net()
{
#if S11
    /*Delaunay DelaunayTri;
    //// ddx, ddy, m_ddz  生成三角形,每个三角形 a b c 三个边, z 值为?
    m_tri_num = DelaunayTri.Triangulate(m_data_num, m_ddx, m_ddy, m_ddz, m_tax, m_tay, m_taz, m_tbx, m_tby, m_tbz, m_tcx, m_tcy, m_tcz);*/
#else
    /* std::vector<delaunay::Point<double>> nps;
     for (int i = 0; i < m_data_num; ++i)
     {
         nps.push_back({ m_ddx[i] , m_ddy[i] });
     }*/
    delaunay::Delaunay<double> result = delaunay::triangulate(m_data_num, m_ddx, m_ddy);
    std::vector<silly_geo_coll> tri_shps;
    for (auto ts : result.triangles)
    {
        silly_geo_coll sgc;
        sgc.m_type = enum_geometry_type::egtPolygon;
        sgc.m_poly.outer_ring.points.push_back({m_ddx[ts.p0.i], m_ddy[ts.p0.i]});
        sgc.m_poly.outer_ring.points.push_back({m_ddx[ts.p1.i], m_ddy[ts.p1.i]});
        sgc.m_poly.outer_ring.points.push_back({m_ddx[ts.p2.i], m_ddy[ts.p2.i]});
        sgc.m_poly.outer_ring.points.push_back({m_ddx[ts.p0.i], m_ddy[ts.p0.i]});
        // sgc.m_props["idx"] = silly_geo_prop(i);
        tri_shps.push_back(sgc);
    }
    geo_utils::write_geo_coll("./iso_tri2.shp", tri_shps);

#endif
}

void silly_iso_line::make_tri_intersect()
{
    //  TODO 这个循环可以 多线程执行, 结果不会相互影响

    int i;
    // #if _OPENMP
    //     #pragma omp parallel for private(i)
    // #endif
    for (i = 0; i < m_thresholds.size(); ++i)
    {
        m_level_tris[i] = {};
        make_tri_intersect(i, m_thresholds[i]);
    }
    SU_INFO_PRINT("分级追踪三角形完成.")
}

void silly_iso_line::make_tri_intersect(const size_t& level, const bool& greater)
{
    double x1, x2, x3;                        // 三角形顶点的x坐标
    double y1, y2, y3;                        // 三角形顶点的y坐标
    double z1, z2, z3;                        // 三角形顶点的数值
    std::vector<iso_triangle> tris_in_level;  // 当前等级被穿过的三角形信息
    double threshold = m_thresholds[level];

    //  这里计算每个for循环中的计算没那么复杂,不适用多线程
    for (int i = 0; i < m_tri_num; i++)
    {
        // 遍历所有三角形获得通过等值线的边数n,并记录三角形的边序号tbb[i],三角形序号tb[i]
        x1 = m_tax[i];
        x2 = m_tbx[i];
        x3 = m_tcx[i];
        y1 = m_tay[i];
        y2 = m_tby[i];
        y3 = m_tcy[i];
        z1 = m_taz[i];
        z2 = m_tbz[i];
        z3 = m_tcz[i];
        // 三个顶点值相等不处理
        if (z1 == z2 && z2 == z3)
        {
            continue;
        }
        // 三个顶点在阈值的同一边,不会相交
        if ((z1 < threshold && z2 < threshold && z3 < threshold) || (z1 > threshold && z2 > threshold && z3 > threshold))
        {
            continue;
        }

        // 只要一条直线线穿过三角形,必定会跟三角形有两个焦点,除非是在顶点上
        iso_triangle tmp_tri;
        tmp_tri.i = i;
        int cont = 0;
        if ((threshold - z1) * (threshold - z2) < 0 && z1 != z2)
        {
            tmp_tri.nBian[0] = 1;                                             // 等值线从AB边穿过
            tmp_tri.dv[0].x = x1 + (x2 - x1) * (threshold - z1) / (z2 - z1);  // 线性插值
            tmp_tri.dv[0].y = y1 + (y2 - y1) * (threshold - z1) / (z2 - z1);
            cont++;
        }
        if ((threshold - z1) * (threshold - z3) < 0 && z1 != z3)
        {
            tmp_tri.nBian[1] = 1;  // 等值线从AC边穿过
            tmp_tri.dv[1].x = x3 + (x1 - x3) * (threshold - z3) / (z1 - z3);
            tmp_tri.dv[1].y = y3 + (y1 - y3) * (threshold - z3) / (z1 - z3);
            cont++;
        }
        if ((threshold - z2) * (threshold - z3) < 0 && z2 != z3)
        {
            tmp_tri.nBian[2] = 1;  // 等值线从BC边穿过
            tmp_tri.dv[2].x = x2 + (x3 - x2) * (threshold - z2) / (z3 - z2);
            tmp_tri.dv[2].y = y2 + (y3 - y2) * (threshold - z2) / (z3 - z2);
            cont++;
        }
        if (cont == 2)
        {
            // #pragma omp atomic
            m_level_tris[level].push_back(tmp_tri);
        }
        else
        {
            int zzz = 0;
        }
    }
}

void silly_iso_line::make_iso_area()
{
    int i;
    // #if _OPENMP
    // #pragma omp parallel for private(i)
    // #endif
    for (i = 0; i < m_thresholds.size(); ++i)
    {
        m_level_polys[i] = {};
        make_iso_area(i);
    }
    SU_INFO_PRINT("分级构建等值面完成.")
}

void silly_iso_line::make_iso_area(const size_t& level)
{
    int ring_num, i, j, k;

    ring_num = 0;
    std::vector<iso_ring> all_rings;
    std::vector<int> tcha(m_level_tris[level].size(), 0);

    int nbian = 0;
    int nlastb = 0;
    int nfirst = 0;
    bool bTrue = true;
    int too_near_skip = 0;
    int trace_tri = 0;
    for (i = 0; i < m_level_tris[level].size(); i++)
    {
        // 搜索闭合等值线
        if (m_level_tris[level][i].traced)
        {
            continue;  // 已被跟踪过
        }

        m_level_tris[level][i].traced = 1;  // 找到一个没有被跟踪过的三角形

        iso_ring tmp_ring;
        trace_ring(i, m_level_tris[level], tmp_ring);  // 追踪出完整环

        // 判断该环 的内外环关系, TODO: 需要验证
        if (point_in_poly(m_tax[m_level_tris[level][i].i], m_tay[m_level_tris[level][i].i], tmp_ring.cpoint))
        {
            if (m_taz[m_level_tris[level][i].i] > m_thresholds[level])
            {
                tmp_ring.outer = 1;
            }
        }
        else
        {
            if (m_taz[m_level_tris[level][i].i] < m_thresholds[level])
            {
                tmp_ring.outer = 1;
            }
        }
        all_rings.push_back(tmp_ring);
        // break;
    }
    // TODO: 这里原本是要搜索下一个面的控制点的,考虑一下以目前的情况来说到底是否需要

    if (!all_rings.empty())
    {
        std::vector<iso_polygon> result;
        draw_iso_fill(result, all_rings.size(), all_rings);
        m_level_polys[level] = result;
    }
}

// 等值面生成
void silly_iso_line::iso_fill(const size_t& level, const size_t& tri_num, std::vector<iso_triangle>& tris)
{
    /* 每个三角形,如果穿过边,那么一定有且仅有两个边被穿过
            1                   1
          -----                    /
        0        0         1      /   0

    */
}

void silly_iso_line::tri_edge(size_t i, size_t bi, double& dx1, double& dx2, double& dy1, double& dy2)
{
    if (bi == 0)
    {
        dx1 = m_tax[i];
        dy1 = m_tay[i];
        dx2 = m_tbx[i];
        dy2 = m_tby[i];
    }
    else if (bi == 1)
    {
        dx1 = m_tax[i];
        dy1 = m_tay[i];
        dx2 = m_tcx[i];
        dy2 = m_tcy[i];
    }
    else
    {
        dx1 = m_tbx[i];
        dy1 = m_tby[i];
        dx2 = m_tcx[i];
        dy2 = m_tcy[i];
    }
}

void silly_iso_line::trace_ring(const size_t& tri_idx, std::vector<iso_triangle>& tri_list, iso_ring& result)
{
    std::cout << tri_idx << std::endl;
    size_t k, tri_edge_idx;
    double dx1, dx2, dy1, dy2;  // 被穿过的边的坐标0
    size_t nlastb = -1;         // 当前闭合曲线中的最新的一个相交三角形的相交边
    size_t nfirst = -1;         // 当前闭合曲线中的第一个一个相交三角形的相交边
    for (k = 0; k < 3; k++)
    {
        if (tri_list[tri_idx].nBian[k] == 1)
        {
            nlastb = k;
            nfirst = k;
            result.cpoint.push_back(tri_list[tri_idx].dv[nlastb]);
        }
    }
    if (nlastb > tri_list.size())
    {
        return;
    }

    tri_edge(tri_list[tri_idx].i, nlastb, dx1, dx2, dy1, dy2);

    bool bTrue = true;
    int draw_cnt = 1;
    while (bTrue)
    {
        bTrue = false;
        // 遍历所有三角形,找到相邻的
        for (auto& tri : tri_list)
        {
            if (tri.traced)
            {
                continue;
            }

            // 有相同的边
            if (compare_triangle_same_edge(dx1, dy1, dx2, dy2, tri.i, tri_edge_idx))
            {
                std::cout << "\t" << tri.i << std::endl;
                tri.traced = 1;
                for (k = 0; k < 3; k++)  // 找到另外一个边
                {
                    if (k != tri_edge_idx && tri.nBian[k])
                    {
                        nlastb = k;
                        break;
                    }
                }

                result.add_c(tri.dv[nlastb], m_near);

                tri_edge(tri.i, nlastb, dx1, dx2, dy1, dy2);
                bTrue = true;
                break;
            }
        }
    }

    // 回填第一个相交点使其闭合
    result.cpoint.push_back(result.cpoint.front());
}

void silly_iso_line::draw_iso_fill(std::vector<iso_polygon>& result_polys, size_t ring_num, std::vector<iso_ring>& all_rings)
{
    // 先将环组合
    // 找到外环
    int out_c = 0;
    for (auto ring : all_rings)
    {
        if (ring.outer)
        {
            iso_polygon poly;
            poly.outer = ring;
            result_polys.emplace_back(poly);
            out_c++;
        }
    }

    // 找到内环
    //  std::map<int, std::vector<int>> poly_inner_rings;
    int int_c = 0;
    for (auto& ring : all_rings)
    {
        if (!ring.outer && !ring.marked)
        {
            for (auto& poly : result_polys)
            {
                if (point_in_poly(ring.cpoint[0].x, ring.cpoint[0].y, poly.outer.cpoint))
                {
                    int_c++;
                    ring.marked = 1;
                    poly.inners.emplace_back(ring);
                }
            }
        }
    }

    int not_c = 0;
    for (auto& ring : all_rings)
    {
        if (!ring.marked && !ring.outer)
        {
            /* iso_polygon poly;
             poly.outer = ring;
             result_polys.emplace_back(poly);*/
            not_c++;
        }
    }

    std::cout << "OUT: " << out_c << std::endl;
    std::cout << "IN: " << int_c << std::endl;
    std::cout << "NOT: " << not_c << std::endl;
    // 开始做平滑

    //// Modified by gaoyang.[2018年5月17日09:53:27]
    //// 这里改为栈内存，之前的20000个点不够，内存越界.
    //// double* map_p = (double*)malloc(20000 * sizeof(double));
    // double* map_p = new double[70000];
    // memset(map_p, 0, 70000 * sizeof(double));

    // gpc* gpc1;
    // gpc1 = new gpc;
    // gpc_polygon clip;

    //// Modified by gaoyang begin.[2018年7月30日16:29:54]
    // if (1 == nClipType)  // 按视口的边框裁剪.
    //{
    //    // 这里按视口范围裁剪.
    //    double tmp_map_b[16];
    //    tmp_map_b[0] = 1;
    //    tmp_map_b[1] = 4;
    //    tmp_map_b[2] = 1;

    //    tmp_map_b[3] = dMinLGTD;
    //    tmp_map_b[4] = dMinLTTD;

    //    tmp_map_b[5] = dMinLGTD;
    //    tmp_map_b[6] = dMaxLTTD;

    //    tmp_map_b[7] = dMaxLGTD;
    //    tmp_map_b[8] = dMaxLTTD;

    //    tmp_map_b[9] = dMaxLGTD;
    //    tmp_map_b[10] = dMinLTTD;

    //    gpc1->gpc_input_polygon(1, &clip, &tmp_map_b[0]);
    //}
    // else if (0 == nClipType)  // 按边界裁剪.
    //{
    //    gpc1->gpc_input_polygon(1, &clip, &map_b[0]);
    //}
    // map_p[0] = 1;
    // map_p[1] = 0;
    // map_p[2] = 0;
    //// Modified by gaoyang begin.[2018年7月30日16:29:54]

    // int nDrawnum = 0;

    // for (auto& poly : result_polys.polys)
    //{
    //    clip_iso_ring(&poly.outer, gpc1, &clip);
    //    for (auto& ring : poly.inners)
    //    {
    //        //该值中共有mian个等值面块
    //        clip_iso_ring(&ring, gpc1, &clip);
    //    }
    //}

    // gpc1->gpc_SU_MEM_FREE_polygon(&clip);
    // delete (gpc1);
    // delete[] map_p;
}

void silly_iso_line::clip_iso_ring(iso_ring* ring, gpc* gpc1, gpc_polygon* clip)
{
    // gpc_polygon subject, result;
    // double *pp = nullptr, *ppp = nullptr;
    // double* pMap;
    // int n, nn, k;

    // int sss = 0;
    // int p;
    // double* map_p = new double[70000];
    // memset(map_p, 0, 70000 * sizeof(double));
    // map_p[0] = 1;
    // map_p[1] = 0;
    // map_p[2] = 0;

    // pMap = map_p + 3;

    // nn = ring->cpoint.size();  //每个面由多少个顶点？
    // if (nn < 3)
    //{
    //    return;  //如果顶点数小于3，处理下一个面
    //}

    // p = 0;
    // sss = 0;
    // pp = (double*)malloc(sizeof(double) * nn * 2);
    // memset(pp, 0, sizeof(double) * nn * 2);
    // ppp = (double*)malloc(sizeof(double) * nn * 2 * 8);
    // memset(ppp, 0, sizeof(double) * nn * 2 * 8);

    // for (k = 0; k < nn; k++)
    //{
    //    pp[k * 2] = ring->cpoint[k].x;      //等值线X坐标
    //    pp[k * 2 + 1] = ring->cpoint[k].y;  //等值线Y坐标
    //}

    // n = nn;
    //// ppp = pp;
    //// line to or move to 当前为曲线
    //// 这里特殊处理了一下点过多的情况，其本身就很平滑.
    // if (k >= 3000)
    //{
    //    cubic_b_spline(0, k - p - 1, (&pp[p * 2]), &n, ppp, false);
    //}
    // else
    //{
    //    cubic_b_spline(0, k - p - 1, (&pp[p * 2]), &n, ppp);
    //}

    // if (n < k - p - 1)
    //{
    //    n = k - p - 1;
    //    memcpy(pMap, (&pp[p * 2]), n * sizeof(double) * 2);
    //}
    // else
    //{
    //    memcpy(pMap, ppp, n * sizeof(double) * 2);
    //}

    // pMap += n * 2;
    // sss += n;

    // if (sss > 2)
    //{
    //    //裁剪
    //    map_p[1] = sss;

    //    gpc1->gpc_input_polygon(1, &subject, &map_p[0]);
    //    subject.hole[0] = 1;
    //    // TODO(gaoyang): 裁剪速度很慢，需要优化.
    //    gpc1->gpc_polygon_clip(GPC_INT, &subject, clip, &result);

    //    for (int c = 0; c < result.num_contours; c++)
    //    {
    //        ring->ppoint.clear();

    //        for (int v = 0; v < result.contour[c].num_vertices; v++)
    //        {
    //            ring->ppoint.push_back(iso_point(result.contour[c].vertex[v].x, result.contour[c].vertex[v].y));
    //        }
    //    }

    //    gpc1->gpc_SU_MEM_FREE_polygon(&subject);
    //    gpc1->gpc_SU_MEM_FREE_polygon(&result);

    //    sss = 0;
    //    pMap = map_p + 3;
    //    SU_MEM_FREE(pp);
    //    SU_MEM_FREE(ppp);
    //}
    // delete[] map_p;
}

// mode  0 闭合 1 线  ni 输入点  poc 坐标  num 输出  buff 输出点  bMultiPoint 一直false
bool silly_iso_line::cubic_b_spline(size_t mode, size_t ni, double* poc, size_t* num, double* buff, const bool bMultiPoint)
{
    double* pp;
    double* p;
    double* b;
    double* c;
    int n, n1, nn, i, j, mm, jj, jj1, jj2, jj3, kk, kk1, kk2, kk3, m1;
    double xm, ym, w, topx, topy, botx, boty, x1, y1, x2, y2, pxt, pyt;
    double pxb, pyb, dl, m, t, t2, t3, ae, bf, cg, dh;
    n = ni;
    *num = 0;
    if (n <= 1)
        return false;
    while (n > 0 && poc[0] == poc[n + n - 2] && poc[1] == poc[n + n - 1])
        n--;
    if (n < 3)
    {
        *num = n;
        for (i = 0; i < 4; i++)
            *(buff + i) = *(poc + i);
        return true;
    }

    // Modified by gaoyang.
    // HANDLE hp;
    // if (!(hp = (HANDLE)GlobalAlloc(GMEM_MOVEABLE,(DWORD)(n*100*sizeof(double))))) return false;
    // if (!(pp = (double*)GlobalLock(hp)))
    //{
    //	GlobalSU_MEM_FREE(hp);
    //	return false;
    //}
    pp = (double*)malloc(n * 100 * sizeof(double));
    if (nullptr == pp)
    {
        return false;
    }

    p = pp + 220;
    b = p + 200;
    c = b + 100;

    n1 = n - 1;

    switch (mode)
    {
        case 0:
            for (i = 0; i < 2 * n; i++)
                pp[i] = (double)poc[i];

            nn = 2 * n;
            topx = pp[nn - 2];
            topy = pp[nn - 1];
            pp[nn] = pp[0];
            pp[nn + 1] = pp[1];
            pp[nn + 2] = pp[2];
            pp[nn + 3] = pp[3];
            botx = pp[2];
            boty = pp[3];
            n1 += 1;
            n += 1;
            break;

        case 1:
            for (i = 0; i < 2 * n; i++)
                pp[i] = (double)poc[i];

            nn = n * 2;
            pp[nn] = pp[0];
            pp[nn + 1] = pp[1];
            x1 = pp[0];
            y1 = pp[1];
            x2 = pp[2];
            y2 = pp[3];
            topx = x1 + x1 - x2;
            if (x2 - x1 == 0)
                topy = y1 + y1 - y2;
            else
                topy = y1 + (topx - x1) * (y2 - y1) / (x2 - x1);
            nn = 2 * n;
            x2 = pp[nn - 4];
            y2 = pp[nn - 3];
            x1 = pp[nn - 2];
            y1 = pp[nn - 1];
            botx = x1 + x1 - x2;
            if (x2 - x1 == 0)
                boty = y1 + y1 - y2;
            else
                boty = y1 + (botx - x1) * (y2 - y1) / (x2 - x1);
            break;
    }

    mm = 0;
    w = 1. / 6.;
    for (j = 0; j < n1; j++)
    {
        jj = 2 * j - 2;
        jj1 = jj + 2;
        jj2 = jj + 4;
        jj3 = jj + 6;
        kk = jj + 1;
        kk1 = kk + 2;
        kk2 = kk + 4;
        kk3 = kk + 6;
        if (j == 0)
        {
            pxt = topx;
            pyt = topy;
            pxb = pp[jj3];
            pyb = pp[kk3];
        }
        else
        {
            pxt = pp[jj];
            pyt = pp[kk];
            if (j == n1 - 1)
            {
                pxb = botx;
                pyb = boty;
            }
            else
            {
                pxb = pp[jj3];
                pyb = pp[kk3];
            }
        }
        xm = fabs((pp[jj1] + pp[jj3]) / 2. - pp[jj2]);
        ym = fabs((pp[kk1] + pp[kk3]) / 2. - pp[kk2]);
        dl = sqrt(xm * xm + ym * ym);
        // if (dl < 1.) dl = 1.;
        m = sqrt(dl / 2.0);
        m1 = (int)(m + .5);
        if (bMultiPoint)
        {
            m1 = m1 > 20 ? 20 : m1;
            m1 = m1 < 3 ? 3 : m1;
        }
        else
        {
            m1 = m1 > 10 ? 10 : m1;
            m1 = m1 < 2 ? 2 : m1;
        }
        m = m1;
        for (i = 0; i <= m1; i++)
        {
            t = i;
            t = t / m;
            t2 = t * t;
            t3 = t2 * t;
            ae = t * (-t2 + 3. * t - 3.) + 1;
            bf = 3. * t2 * (t - 2.) + 4.;
            cg = 3. * t * (-t2 + t + 1.) + 1;
            dh = t3;
            *(buff + mm) = (ae * pxt + bf * pp[jj1] + cg * pp[jj2] + dh * pxb) * w;
            *(buff + mm + 1) = (ae * pyt + bf * pp[kk1] + cg * pp[kk2] + dh * pyb) * w;
            mm += 2;
            if ((mm >= 4) && (buff[mm - 2] == buff[mm - 4]) && (buff[mm - 1] == buff[mm - 3]))
                mm -= 2;
        }
    }
    *num = mm / 2;

    // GlobalUnlock(hp);
    // GlobalSU_MEM_FREE(hp);
    SU_MEM_FREE(pp);
    pp = nullptr;
    return true;
}

void silly_iso_line::reset()
{
    m_level_tris.clear();
    m_level_polys.clear();
    // 等级阈值
    m_thresholds.clear();
}

void silly_iso_line::clear()
{
    SU_MEM_FREE(m_ddx);
    SU_MEM_FREE(m_ddy);
    SU_MEM_FREE(m_ddz);
    SU_MEM_FREE(m_tax);
    SU_MEM_FREE(m_tay);
    SU_MEM_FREE(m_tbx);
    SU_MEM_FREE(m_tby);
    SU_MEM_FREE(m_tcx);
    SU_MEM_FREE(m_tcy);
    SU_MEM_FREE(m_taz);
    SU_MEM_FREE(m_tbz);
    SU_MEM_FREE(m_tcz);
}

bool silly_iso_line::compare_triangle_same_edge(double dx1, double dy1, double dx2, double dy2, iso_point* point, size_t& nBian)
{
    for (int c = 0; c < 3; c++)
    {
        if (dx1 == point[c].x && dy1 == point[c].y)
        {
            for (int cc = 0; cc < 3; cc++)
            {
                if (cc == c)
                {
                    continue;
                }
                if (dx2 == point[cc].x && dy2 == point[cc].y)
                {
                    nBian = cc + c - 1;
                    return true;
                }
            }
        }
    }
    return false;
}

bool silly_iso_line::compare_triangle_same_edge(double dx1, double dy1, double dx2, double dy2, size_t& i, size_t& nBian)
{
    if (dx1 == m_tax[i] && dy1 == m_tay[i] && dx2 == m_tbx[i] && dy2 == m_tby[i])
    {
        nBian = 0;
        return true;
    }

    if (dx1 == m_tbx[i] && dy1 == m_tby[i] && dx2 == m_tcx[i] && dy2 == m_tcy[i])
    {
        nBian = 1;
        return true;
    }

    if (dx1 == m_tcx[i] && dy1 == m_tcy[i] && dx2 == m_tax[i] && dy2 == m_tay[i])
    {
        nBian = 2;
        return true;
    }

    return false;
}

std::string silly_iso_line::geojson(const std::string& path)
{
    std::vector<silly_geo_coll> tri_shps;
    for (int i = 0; i < m_tri_num; ++i)
    {
        silly_geo_coll sgc;
        sgc.m_type = enum_geometry_type::egtPolygon;
        sgc.m_poly.outer_ring.points.push_back({m_tax[i], m_tay[i]});
        sgc.m_poly.outer_ring.points.push_back({m_tbx[i], m_tby[i]});
        sgc.m_poly.outer_ring.points.push_back({m_tcx[i], m_tcy[i]});
        sgc.m_poly.outer_ring.points.push_back({m_tax[i], m_tay[i]});
        sgc.m_props["idx"] = silly_geo_prop(i);
        tri_shps.push_back(sgc);
    }
    geo_utils::write_geo_coll("./iso_tri.shp", tri_shps);

    std::vector<silly_geo_coll> iso_shps;
    for (auto [l, ps] : m_level_polys)
    {
        for (auto p : ps)
        {
            silly_geo_coll sgc;
            sgc.m_type = enum_geometry_type::egtPolygon;
            for (auto cp : p.outer.cpoint)
            {
                sgc.m_poly.outer_ring.points.push_back({cp.x, cp.y});
            }
            sgc.m_props["level"] = silly_geo_prop((int)l);
            iso_shps.push_back(sgc);
        }
    }
    geo_utils::write_geo_coll("./iso_area.shp", iso_shps);

    std::vector<silly_geo_coll> point_shps;
    // for (auto [l, ps] : m_level_polys)
    {
        for (int i = 0; i < m_data_num; ++i)
        {
            silly_geo_coll sgc;
            sgc.m_type = enum_geometry_type::egtPoint;
            sgc.m_point = {m_ddx[i], m_ddy[i]};
            sgc.m_props["value"] = silly_geo_prop(m_ddz[i]);
            point_shps.push_back(sgc);
        }
    }
    geo_utils::write_geo_coll("./iso_point.shp", point_shps);
    return std::string();
}

int silly_iso_line::point_in_poly(const double& x, const double& y, const std::vector<iso_point>& points)
{
    int i, j;
    double d;
    int c = 0;
    int pnum = points.size();
    for (i = 0, j = pnum - 1; i < pnum; j = i++)
    {
        d = (points[j].x - points[i].x) * (y - points[i].y) / (points[j].y - points[i].y) + points[i].x;
        if (d != d && x == d)
        {
            return 0;
        }
        if ((((points[i].y <= y) && (y < points[j].y) || ((points[j].y <= y) && (y < points[i].y))) && (x < d)))
        {
            c = !c;
        }
    }
    return c;
}