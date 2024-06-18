//
// Created by dell on 2023/12/6.
//

#include "silly_vector_to_raster.h"
#include "su_marco.h"
#include <algorithm>
#include <math.h>

#define SILLY_SV2R_CHECK_INTERSECT_POINT(a, b)                              \
    SV2RPoint v1 = vertices[a];                                             \
    SV2RPoint v2 = vertices[b];                                             \
    if ((scanY >= v1.y && scanY < v2.y) || (scanY >= v2.y && scanY < v1.y)) \
    {                                                                       \
        float slope = (v2.x - v1.x) / (v2.y - v1.y);                        \
        float x = (scanY - v1.y) * slope + v1.x;                            \
        edges.push_back(std::round(x));                                     \
    }

bool xscan_line_raster::init()
{
    return false;
}

void xscan_line_raster::reset()
{
    ncols = 0;
    nrows = 0;
    xllcorner = 0;
    yllcorner = 0.;
    cell_size = 0.000001;
    row_pairs.clear();
    ;
}

#define SILLY_XSCAN_LINE_CHECK_POINT(point)                  \
    int tmp_x = std::round((point.lgtd - left) / cell_size); \
    int tmp_y = std::round((top - point.lttd) / cell_size);  \
    if (last_x != tmp_x || last_y != tmp_y)                  \
    {                                                        \
        tmp_vertices.push_back(SV2RPoint(tmp_x, tmp_y));     \
        last_x = tmp_x;                                      \
        last_y = tmp_y;                                      \
    }

bool xscan_line_raster::rasterization(const silly_multi_poly& m_polys)
{
    std::vector<std::vector<SV2RPoint>> vertices_arr;
    double left = 99999., right = -99999., top = -99999.0, bottom = 99999.;
    // 找出上下左右
    for (const auto& poly : m_polys)
    {
        for (const auto& ring : poly.inner_rings)
        {
            for (const auto& point : ring.points)
            {
                left = SU_MIN(point.lgtd, left);
                right = SU_MAX(point.lgtd, right);
                top = SU_MAX(point.lttd, top);
                bottom = SU_MIN(point.lttd, bottom);
            }
        }
        for (const auto& point : poly.outer_ring.points)
        {
            left = SU_MIN(point.lgtd, left);
            right = SU_MAX(point.lgtd, right);
            top = SU_MAX(point.lttd, top);
            bottom = SU_MIN(point.lttd, bottom);
        }
    }

    left = std::floor(left / cell_size) * cell_size;
    right = std::ceil(right / cell_size) * cell_size;
    top = std::ceil(top / cell_size) * cell_size;
    bottom = std::floor(bottom / cell_size) * cell_size;

    xllcorner = left;
    yllcorner = bottom;

    nrows = std::ceil((top - bottom) / cell_size);
    ncols = std::ceil((right - left) / cell_size);

    for (const auto& poly : m_polys)
    {
        // 由于精度(cell_size)问题,可能会导致一个矢量中有大量重复的连续栅格点,将这部分连续栅格点归并为一个
        int last_x = 0 - ncols;
        int last_y = 0 - nrows;
        for (const auto& ring : poly.inner_rings)
        {
            std::vector<SV2RPoint> tmp_vertices;
            for (const auto& point : ring.points)
            {
                SILLY_XSCAN_LINE_CHECK_POINT(point);
            }
            vertices_arr.push_back(tmp_vertices);
        }
        std::vector<SV2RPoint> tmp_vertices;

        for (const auto& point : poly.outer_ring.points)
        {
            SILLY_XSCAN_LINE_CHECK_POINT(point);
        }
        vertices_arr.push_back(tmp_vertices);
    }

    return rasterization(vertices_arr);
}

bool xscan_line_raster::rasterization(const std::vector<std::vector<SV2RPoint>> vertices_arr)
{
    int minY = 99999, maxY = 0;
    int minX = 99999, maxX = 0;

    for (const auto& part : vertices_arr)
    {
        for (const auto& point : part)
        {
            minY = SU_MAX(0, SU_MIN(minY, point.y));
            maxY = SU_MAX(0, SU_MAX(maxY, point.y));
            minX = SU_MAX(0, SU_MIN(minX, point.x));
            maxX = SU_MAX(0, SU_MAX(maxX, point.x));
        }
    }
    if (!nrows || !ncols)
    {
        nrows = maxY - minY + 1;
        ncols = maxX - minX + 1;
    }

    // 对每一条扫描线进行处理
    for (int scanY = minY; scanY <= maxY; ++scanY)
    {
        // 构建边缘列表
        std::vector<int> edges;
        for (int part = 0; part < vertices_arr.size(); ++part)
        {
            auto vertices = vertices_arr[part];
            int numVertices = vertices.size();
            for (int i = 0; i < numVertices; ++i)
            {
                SV2RPoint v1 = vertices[i];
                SV2RPoint v2 = vertices[(i + 1) % numVertices];
                if ((scanY >= v1.y && scanY < v2.y) || (scanY >= v2.y && scanY < v1.y))
                {
                    float slope = (v2.x - v1.x) / (v2.y - v1.y);
                    float x = (scanY - v1.y) * slope + v1.x;
                    edges.push_back(SU_MAX(0, static_cast<int>(x)));
                }
            }
        }
        // 根据X值对边缘进行排序
        std::sort(edges.begin(), edges.end());
        int old = 0;
        for (int i = 0; i < edges.size(); i += 2)
        {
            SV2RPair tmp_pair;
            tmp_pair.beg = edges[i];
            tmp_pair.end = edges[i + 1];
            row_pairs[scanY].push_back(tmp_pair);
        }
    }
    return true;
}

bool xscan_line_raster::rasterization(const silly_geo_coll& geo_coll)
{
    return false;
}

#ifndef NDEBUG
#include <image/png_utils.h>
using namespace silly_image;
#endif

void xscan_line_raster::image(const std::string& path)
{
#ifndef NDEBUG
    png_data png = png_utils::create_empty(nrows, ncols, PNG_COLOR_TYPE_GRAY);

    for (int r = 0; r < nrows; ++r)
    {
        int old = 0;
        for (auto p : row_pairs[r])
        {
            for (int i = p.beg; i < p.end; ++i)
            {
                png.set_pixel(r, i, silly_color(240));
            }
            old = p.end;
        }
    }

    png_utils::write(path.c_str(), png);
#endif
}
