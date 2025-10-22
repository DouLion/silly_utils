/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-14
 * @file: silly_dem.c
 * @description: silly_dem实现
 * @version: v1.0.1 2025-08-14 dou li yang
 */
#include "silly_dem.h"

#include "geo/silly_geo_utils.h"
#define CVT_CHECK_G2LL_BOUND(l0, x0, y0, x1, y1, rect0)             \
    silly::geo::proj::convert::gauss_to_lonlat(l0, x0, y0, x1, y1); \
    rect0.min.x = std::min(x1, rect0.min.x);                        \
    rect0.min.y = std::min(y1, rect0.min.y);                        \
    rect0.max.x = std::max(x1, rect0.max.x);                        \
    rect0.max.y = std::max(y1, rect0.max.y);

#define CVT_CHECK_LL2G_BOUND(l0, x0, y0, x1, y1, rect0)             \
    silly::geo::proj::convert::lonlat_to_gauss(l0, x0, y0, x1, y1); \
    rect0.min.x = std::min(x1, rect0.min.x);                        \
    rect0.min.y = std::min(y1, rect0.min.y);                        \
    rect0.max.x = std::max(x1, rect0.max.x);                        \
    rect0.max.y = std::max(y1, rect0.max.y);


bool suDem::IsGauss() const
{
    static std::set<int> valid = {75, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108, 111, 114, 117, 120, 123, 126, 129, 132, 135};
    return valid.find(static_cast<int>(info.central)) != std::end(valid);

}

void suDem::Gauss2Lonlat(const suDem& rh, const double& cell_size, const double& l0)
{
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    // TODO: 需要先检查 是否为高斯
    info.bound.min.x = -999.0;
    info.bound.max.x = -999.0;
    info.bound.min.y = -999.0;
    info.bound.max.y = -999.0;
    double lon, lat;
    CVT_CHECK_G2LL_BOUND(l0, rh.info.bound.min.x, rh.info.bound.max.y, lon, lat, info.bound)
    CVT_CHECK_G2LL_BOUND(l0, rh.info.bound.min.x, rh.info.bound.min.y, lon, lat, info.bound)
    CVT_CHECK_G2LL_BOUND(l0, rh.info.bound.max.x, rh.info.bound.max.y, lon, lat, info.bound)
    CVT_CHECK_G2LL_BOUND(l0, rh.info.bound.max.x, rh.info.bound.min.y, lon, lat, info.bound)
    info.height = std::round((info.bound.max.y - info.bound.min.y) / cell_size);
    info.width = std::round(((info.bound.max.x - info.bound.min.x) / cell_size));
    info.dx = cell_size;
    info.dy = cell_size;
    raster.create(info.height, info.width, true);
    raster.set(rh.info.fill);
    double* dp = raster.data();
    for (int64_t r = 0; r < info.height; r++)
    {
        for (int64_t c = 0; c < info.width; c++)
        {
            double gx, gy;
            lon = info.bound.min.x + c * info.dx;
            lat = info.bound.max.y - r * info.dy;
            LONLAT2GAUSS(l0, lon, lat, gx, gy);
            int64_t gC = std::round((gx - rh.info.bound.min.x) / rh.info.dx);
            int64_t gR = std::round((gy - rh.info.bound.min.y) / rh.info.dy);
            if (gR >= 0 && gR < rh.raster.row() && gC >= 0 && gC < rh.raster.col())
            {
                const int64_t gI = gR * rh.raster.col() + gC;
                *dp = rh.raster.data()[gI];
            }
            dp++;
        }
    }
}
void suDem::Lonlat2Gauss(const suDem& rh, const double& cell_size, const double& l0)
{
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    // TODO: 需要先检查 是否为高斯
    info.bound.min.x = 1.e10;
    info.bound.max.x = -1.e10;
    info.bound.min.y = 1.e10;
    info.bound.max.y = -1.e10;
    double gx, gy;
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.min.x, rh.info.bound.max.y, gx, gy, info.bound)
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.min.x, rh.info.bound.min.y, gx, gy, info.bound)
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.max.x, rh.info.bound.max.y, gx, gy, info.bound)
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.max.x, rh.info.bound.min.y, gx, gy, info.bound)
    info.height = std::round((info.bound.max.y - info.bound.min.y) / cell_size);
    info.width = std::round(((info.bound.max.x - info.bound.min.x) / cell_size));
    info.dx = cell_size;
    info.dy = cell_size;
    info.central = l0;
    raster.create(info.height, info.width, true);
    raster.set(rh.info.fill);
    double* dp = raster.data();
    for (int64_t r = 0; r < info.height; r++)
    {
        for (int64_t c = 0; c < info.width; c++)
        {
            double lon, lat;
            gx = info.bound.min.x + c * info.dx;
            gy = info.bound.max.y - r * info.dy;
            GAUSS2LONLAT(info.central, gx, gy, lon, lat);
            int64_t llC = std::round((lon - rh.info.bound.min.x) / rh.info.dx);
            int64_t llR = std::round((rh.info.bound.max.y - lat) / rh.info.dy);
            if (llR >= 0 && llR < rh.raster.row() && llC >= 0 && llC < rh.raster.col())
            {
                const int64_t llI = llR * rh.raster.col() + llC;
                *dp = rh.raster.data()[llI];
            }
            dp++;
        }
    }
}

void suDem::Cover(const suDem& rh)
{
    int xOff = 0, yOff = 0;

    for (int r = 0; r < rh.raster.row(); r++)
    {
        for (int c = 0; c < rh.raster.col(); c++)
        {
            int ty = r + yOff;
            int tx = c + xOff;
            if (ty >= 0 && ty < raster.row() && tx >= 0 && tx < raster.col())
            {
                raster[ty][tx] = rh.raster[r][c];
            }
        }
    }
}
void suDem::Extra(const silly_ring& ring)
{
    double* ptr = raster.data();
    if (!ptr)
    {
        return;
    }
    for (int r = 0; r < info.height; ++ r)
    {
        for (int c = 0; c < info.width; ++ c)
        {
            silly_point p;
            p.x = info.bound.min.x + c * info.dx;
            p.y = info.bound.min.y + r * info.dy;
            if (!sugeoutils::intersect(p, ring.points)) // 点在环外
            {
                *ptr = info.fill;
            }
        }
    }
}
void suDem::Extra(const suDem& rh, const silly_ring& ring)
{
    if (this != &rh)
    {
        info = rh.info;
        raster.create(info.height, info.width, true);
    }

    Extra(ring);
}
std::vector<std::pair<double, double>> suDem::ProfileElev(const silly_line& line) const
{
    // line等间距划分
    std::vector<std::pair<double, double>> ret;
    double dist = (info.dx + info.dy)/4;
    silly_line eqLine = line.equidistant(dist);
    double xl = info.bound.min.x - dist * info.dx;
    double yl = info.bound.max.y - dist * info.dy;
    for (size_t i = 0; i < eqLine.size(); i++)
    {
        const auto& p = eqLine[i];
        double r = (info.bound.max.y - p.y) / yl;
        double c = (info.bound.max.x - p.x) / xl;
        double v = raster.bilinear(r, c);
        ret.push_back({i*dist, v});
    }
    return ret;
}

void suDem::Release()
{
    raster.release();
}