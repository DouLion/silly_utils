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

void sudem::gauss2lonlat(const sudem& rh, const double& cellsize, const double& l0)
{
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    // TODO: 需要先检查 是否为高斯
    m_rect.min.x = -999.0;
    m_rect.max.x = -999.0;
    m_rect.min.y = -999.0;
    m_rect.max.y = -999.0;
    double lon, lat;
    CVT_CHECK_G2LL_BOUND(l0, rh.m_rect.min.x, rh.m_rect.max.y, lon, lat, m_rect)
    CVT_CHECK_G2LL_BOUND(l0, rh.m_rect.min.x, rh.m_rect.min.y, lon, lat, m_rect)
    CVT_CHECK_G2LL_BOUND(l0, rh.m_rect.max.x, rh.m_rect.max.y, lon, lat, m_rect)
    CVT_CHECK_G2LL_BOUND(l0, rh.m_rect.max.x, rh.m_rect.min.y, lon, lat, m_rect)
    m_height = std::round((m_rect.max.y - m_rect.min.y) / cellsize);
    m_width = std::round(((m_rect.max.x - m_rect.min.x) / cellsize));
    m_dx = cellsize;
    m_dy = cellsize;
    m_data.create(m_height, m_width, true);
    m_data.set(rh.m_fill);
    double* dp = m_data.data();
    for (int64_t r = 0; r < m_height; r++)
    {
        for (int64_t c = 0; c < m_width; c++)
        {
            double gx, gy;
            lon = m_rect.min.x + c * m_dx;
            lat = m_rect.max.y - r * m_dy;
            LONLAT2GAUSS(l0, lon, lat, gx, gy);
            int64_t gC = std::round((gx - rh.m_rect.min.x) / rh.m_dx);
            int64_t gR = std::round((gy - rh.m_rect.min.y) / rh.m_dy);
            if (gR >= 0 && gR < rh.m_data.row() && gC >= 0 && gC < rh.m_data.col())
            {
                const int64_t gI = gR * rh.m_data.col() + gC;
                *dp = rh.m_data.data()[gI];
            }
            dp++;
        }
    }
}
void sudem::lonlat2gauss(const sudem& rh, const double& cellsize, const double& l0)
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
    m_rect.min.x = 1.e10;
    m_rect.max.x = -1.e10;
    m_rect.min.y = 1.e10;
    m_rect.max.y = -1.e10;
    double gx, gy;
    CVT_CHECK_LL2G_BOUND(l0, rh.m_rect.min.x, rh.m_rect.max.y, gx, gy, m_rect)
    CVT_CHECK_LL2G_BOUND(l0, rh.m_rect.min.x, rh.m_rect.min.y, gx, gy, m_rect)
    CVT_CHECK_LL2G_BOUND(l0, rh.m_rect.max.x, rh.m_rect.max.y, gx, gy, m_rect)
    CVT_CHECK_LL2G_BOUND(l0, rh.m_rect.max.x, rh.m_rect.min.y, gx, gy, m_rect)
    m_height = std::round((m_rect.max.y - m_rect.min.y) / cellsize);
    m_width = std::round(((m_rect.max.x - m_rect.min.x) / cellsize));
    m_dx = cellsize;
    m_dy = cellsize;
    m_l0 = l0;
    m_data.create(m_height, m_width, true);
    m_data.set(rh.m_fill);
    double* dp = m_data.data();
    for (int64_t r = 0; r < m_height; r++)
    {
        for (int64_t c = 0; c < m_width; c++)
        {
            double lon, lat;
            gx = m_rect.min.x + c * m_dx;
            gy = m_rect.max.y - r * m_dy;
            GAUSS2LONLAT(m_l0, gx, gy, lon, lat);
            int64_t llC = std::round((lon - rh.m_rect.min.x) / rh.m_dx);
            int64_t llR = std::round((rh.m_rect.max.y -lat) / rh.m_dy);
            if (llR >= 0 && llR < rh.m_data.row() && llC >= 0 && llC < rh.m_data.col())
            {
                const int64_t llI = llR * rh.m_data.col() + llC;
                *dp = rh.m_data.data()[llI];
            }
            dp++;
        }
    }
}

void sudem::cover(const sudem& rh)
{
    
    int xOff = 0, yOff = 0;

    for (int r = 0; r < rh.m_data.row(); r++)
    {
        for (int c = 0; c < rh.m_data.col(); c++)
        {
            int ty = r + yOff;
            int tx = c + xOff;
            if (ty >= 0 && ty < m_data.row() && tx >= 0 && tx < m_data.col())
            {
                m_data[ty][tx] = rh.m_data[r][c];
            }
        }
    }
}
