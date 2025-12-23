/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-23
 * @file: silly_grid_algo.c
 * @description: silly_grid_algo实现
 * @version: v1.0.1 2025-12-23 dou li yang
 */
#include "silly_grid_algo.h"
float suGridAlgo::SummaryPoint(const silly_tzx_grid& tgrd, const suPoint& p, const uint32_t& kn, const summary_func& func)
{
    const uint32_t n = SU_MAX(kn, 1);
    const suRect bd = tgrd.rect();
    const int r = std::round((bd.max.y - p.y) / (bd.max.y - bd.min.y) * tgrd.rows());
    const int c = std::round((p.x - bd.min.x) / (bd.max.x - bd.min.x) * tgrd.cols());
    suFMatrix mtx = tgrd.frame(0);
    return SummaryPoint(mtx, r, c, n, func);
}

float suGridAlgo::SummaryPoint(const suFMatrix& mtx, const int& r, const int& c, const uint32_t& kn, const summary_func& func)
{
    std::vector<float> vec;
    int br = SU_MIN(SU_MAX(r - kn, 0), mtx.rows() - 1);
    int bc = SU_MIN(SU_MAX(c - kn, 0), mtx.cols() - 1);
    int er = SU_MIN(SU_MAX(r + kn, 0), mtx.rows() - 1);
    int ec = SU_MIN(SU_MAX(c + kn, 0), mtx.cols() - 1);
    for (int i = br; i <= er; i++)
    {
        for (int j = bc; j <= ec; j++)
        {
            if (i >= 0 && i < mtx.rows() && j >= 0 && j < mtx.cols())
            {
                vec.push_back(mtx.at(i, j));
            }
        }
    }
    const float ret = func(vec);
    return ret;
}
float suGridAlgo::SummaryMultiPoly(const silly_tzx_grid& tgrd, const suMultiPoly& mp, const summary_func& func)
{
    const suRect bd = tgrd.rect();
    const double cell_size = (tgrd.xdelta() + tgrd.ydelta()) / 2.0;
    XScanLine xsl;
    xsl.set(bd, cell_size);
    xsl.rasterize(mp);
    xsl.fill();
    const XScanLine::ScanPairs sps = xsl.row_pairs();
    suFMatrix mtx = tgrd.frame(0);
    return SummaryMultiPoly(mtx, sps, func);
}
float suGridAlgo::SummaryMultiPoly(const suFMatrix& mtx, const XScanLine::ScanPairs& sps, const summary_func& func)
{
    std::vector<float> vec;
    for (const auto& [r, ps] : sps)
    {
        for (const auto& p : ps)
        {
            for (int c = p.beg; c <= p.end; c++)
            {
                vec.push_back(mtx.at(r, c));
            }
        }
    }

    const float ret = func(vec);
    return ret;
}