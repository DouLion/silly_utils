//
// Created by dell on 2023/12/6.
//

#include "silly_vector_to_raster.h"
#include "su_marco.h"
#include <algorithm>
#include <math.h>

#define SILLY_SV2R_CHECK_INTERSECT_POINT(a, b)                              \
    raster_point v1 = vertices[a];                                          \
    raster_point v2 = vertices[b];                                          \
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
    left = 0;
    top = 0;
    bottom = 0;
    right = 0;
    cell_size = 0.000001;
    row_pairs.clear();
}

#define SILLY_XSCAN_LINE_CHECK_POINT(point)                  \
    int tmp_x = std::round((point.lgtd - left) / cell_size); \
    int tmp_y = std::round((top - point.lttd) / cell_size);  \
    if (last_x != tmp_x || last_y != tmp_y)                  \
    {                                                        \
        tmp_vertices.push_back(raster_point(tmp_x, tmp_y));  \
        last_x = tmp_x;                                      \
        last_y = tmp_y;                                      \
    }

bool xscan_line_raster::remove_repeat_row_pairs()
{
    for (auto& [key, pairs] : row_pairs)
    {
        // 使用 std::set 来自动去重
        std::set<cover_pair> unique_pairs(pairs.begin(), pairs.end());
        // 清空源数据将去重后的结果复制回原 vector
        pairs.assign(unique_pairs.begin(), unique_pairs.end());
    }
    return true;
}

bool xscan_line_raster::rasterization_point_algo(const silly_point& point)
{
    int raster_x = std::round((point.lgtd - left) / cell_size);
    int raster_y = std::round((top - point.lttd) / cell_size);
    if (raster_x >= 0 && raster_x <= ncols && raster_y >= 0 && raster_y <= nrows)
    {
        // 由于只有只有一个点
        cover_pair tmp_pair;
        tmp_pair.beg = raster_x;
        tmp_pair.end = raster_x;
        row_pairs[raster_y].push_back(tmp_pair);
        return true;
    }
    return false;
}

bool xscan_line_raster::rasterization(const silly_point& point)
{
    nrows = std::ceil((top - bottom) / cell_size);
    ncols = std::ceil((right - left) / cell_size);

    bool status = rasterization_point_algo(point);

    return status;
}

bool xscan_line_raster::rasterization(const silly_multi_point& points)
{
    // 计算所有点的边界
    if (std::abs(left) < SU_EPSILON && std::abs(top) < SU_EPSILON && std::abs(right) < SU_EPSILON && std::abs(bottom) < SU_EPSILON)
    {
        // 遍历所有点来确定边界
        for (const auto& point : points)
        {
            left = SU_MIN(point.lgtd, left);
            right = SU_MAX(point.lgtd, right);
            top = SU_MAX(point.lttd, top);
            bottom = SU_MIN(point.lttd, bottom);
        }

        // 调整边界以与网格对齐
        left = std::floor(left / cell_size) * cell_size;
        right = std::ceil(right / cell_size) * cell_size;
        top = std::ceil(top / cell_size) * cell_size;
        bottom = std::floor(bottom / cell_size) * cell_size;
    }

    // 计算栅格大小
    nrows = std::ceil((top - bottom) / cell_size);
    ncols = std::ceil((right - left) / cell_size);

    int last_x = 0 - ncols;
    int last_y = 0 - nrows;

    for (const auto& point : points)
    {
        rasterization_point_algo(point);
    }

    // 去重row_pairs
    remove_repeat_row_pairs();

    return true;
}

bool xscan_line_raster::rasterization_line_algo(const silly_line& line)
{
    if (line.size() < 2)
    {
        return false;  // 如果线段小于两个点，则无法处理
    }
    // 遍历线段中的每一对点
    for (size_t i = 0; i < line.size() - 1; ++i)
    {
        const silly_point& p1 = line[i];
        const silly_point& p2 = line[i + 1];

        // 将经纬度转换为栅格列和行
        int col1 = std::round((p1.lgtd - left) / cell_size);
        int row1 = std::round((top - p1.lttd) / cell_size);
        int col2 = std::round((p2.lgtd - left) / cell_size);
        int row2 = std::round((top - p2.lttd) / cell_size);

        // 处理垂直线段
        if (col1 == col2)
        {
            int start_row = std::min(row1, row2);
            int end_row = std::max(row1, row2);
            for (int row = start_row; row <= end_row; ++row)
            {
                if (row >= 0 && row < nrows)
                {
                    row_pairs[row].push_back({col1, col1});
                }
            }
            continue;
        }

        // 处理非垂直线段
        if (row1 == row2)
        {
            int beg = std::min(col1, col2);
            int end = std::max(col1, col2);
            if (beg >= 0 && beg < ncols && end >= 0 && end < ncols)
            {
                row_pairs[row1].push_back({beg, end});
            }
            continue;
        }

        // 计算斜率和截距
        float slope = (p2.lttd - p1.lttd) / (p2.lgtd - p1.lgtd);
        float intercept = p1.lttd - slope * p1.lgtd;

        int min_col = std::min(col1, col2);  // 最小x
        int max_col = std::max(col1, col2);  // 最大x

        int min_row = std::min(row1, row2);  // 最小y
        int max_row = std::max(row1, row2);  // 最大y

        // 对于每一列，找到对应的行
        for (int col = min_col; col <= max_col; ++col)
        {
            // 使用斜率和截距计算该列对应的行
            float y = slope * ((col + 0.5) * cell_size + left) + intercept;  // 使用0.5是为了确保点在栅格中心
            int row = std::round((top - y) / cell_size);
            // 确保行在范围内
            if (row >= min_row && row <= max_row)
            {
                // 添加该点到对应行的列表中
                row_pairs[row].push_back({col, col});
            }
        }
    }

    return true;
}

bool xscan_line_raster::rasterization_line(const silly_line& line)
{
    if (std::abs(left) < SU_EPSILON && std::abs(top) < SU_EPSILON && std::abs(right) < SU_EPSILON && std::abs(bottom) < SU_EPSILON)
    {
        // 查找边界框
        for (const auto& point : line)
        {
            left = SU_MIN(point.lgtd, left);
            right = SU_MAX(point.lgtd, right);
            top = SU_MAX(point.lttd, top);
            bottom = SU_MIN(point.lttd, bottom);
        }

        left = std::floor(left / cell_size) * cell_size;
        right = std::ceil(right / cell_size) * cell_size;
        top = std::ceil(top / cell_size) * cell_size;
        bottom = std::floor(bottom / cell_size) * cell_size;
    }

    nrows = std::ceil((top - bottom) / cell_size);
    ncols = std::ceil((right - left) / cell_size);

    rasterization_line_algo(line);

    // 去重row_pairs
    remove_repeat_row_pairs();

    return true;
}

bool xscan_line_raster::rasterization(const silly_multi_silly_line& lines)
{
    std::vector<std::vector<raster_point>> vertices_arr;

    if (std::abs(left) < SU_EPSILON && std::abs(top) < SU_EPSILON && std::abs(right) < SU_EPSILON && std::abs(bottom) < SU_EPSILON)
    {
        // 查找边界框
        for (const auto& line : lines)
        {
            for (const auto& point : line)
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
    }

    nrows = std::ceil((top - bottom) / cell_size);
    ncols = std::ceil((right - left) / cell_size);

    for (const auto& line : lines)
    {
        rasterization_line_algo(line);
    }

    // 去重row_pairs
    remove_repeat_row_pairs();

    return true;
}

bool xscan_line_raster::rasterization(const silly_poly& poly)
{
    std::vector<std::vector<raster_point>> vertices_arr;

    // 必要时初始化边界框
    if (std::abs(left) < SU_EPSILON && std::abs(top) < SU_EPSILON && std::abs(right) < SU_EPSILON && std::abs(bottom) < SU_EPSILON)
    {
        // 查找边界框
        for (const auto& point : poly.outer_ring.points)
        {
            left = SU_MIN(point.lgtd, left);
            right = SU_MAX(point.lgtd, right);
            top = SU_MAX(point.lttd, top);
            bottom = SU_MIN(point.lttd, bottom);
        }

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

        left = std::floor(left / cell_size) * cell_size;
        right = std::ceil(right / cell_size) * cell_size;
        top = std::ceil(top / cell_size) * cell_size;
        bottom = std::floor(bottom / cell_size) * cell_size;
    }

    nrows = std::ceil((top - bottom) / cell_size);
    ncols = std::ceil((right - left) / cell_size);

    // 将点转换为光栅坐标并合并连续的相同点
    int last_x = 0 - ncols;
    int last_y = 0 - nrows;
    std::vector<raster_point> tmp_vertices;

    for (const auto& point : poly.outer_ring.points)
    {
        SILLY_XSCAN_LINE_CHECK_POINT(point);
    }
    vertices_arr.push_back(tmp_vertices);

    for (const auto& ring : poly.inner_rings)
    {
        tmp_vertices.clear();
        last_x = 0 - ncols;
        last_y = 0 - nrows;
        for (const auto& point : ring.points)
        {
            SILLY_XSCAN_LINE_CHECK_POINT(point);
        }
        vertices_arr.push_back(tmp_vertices);
    }

    return rasterization(vertices_arr);
}

bool xscan_line_raster::rasterization(const silly_multi_poly& m_polys)
{
    std::vector<std::vector<raster_point>> vertices_arr;
    if (std::abs(left) < SU_EPSILON && std::abs(top) < SU_EPSILON && std::abs(right) < SU_EPSILON && std::abs(bottom) < SU_EPSILON)
    {
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
    }

    nrows = std::ceil((top - bottom) / cell_size);
    ncols = std::ceil((right - left) / cell_size);

    for (const auto& poly : m_polys)
    {
        // 由于精度(cell_size)问题,可能会导致一个矢量中有大量重复的连续栅格点,将这部分连续栅格点归并为一个
        int last_x = 0 - ncols;
        int last_y = 0 - nrows;
        for (const auto& ring : poly.inner_rings)
        {
            std::vector<raster_point> tmp_vertices;
            for (const auto& point : ring.points)
            {
                SILLY_XSCAN_LINE_CHECK_POINT(point);
            }
            vertices_arr.push_back(tmp_vertices);
        }
        std::vector<raster_point> tmp_vertices;

        for (const auto& point : poly.outer_ring.points)
        {
            SILLY_XSCAN_LINE_CHECK_POINT(point);
        }
        vertices_arr.push_back(tmp_vertices);
    }

    return rasterization(vertices_arr);
}

bool xscan_line_raster::rasterization(const std::vector<std::vector<raster_point>> vertices_arr)
{
    int minY = INT_MAX, maxY = 0;
    int minX = INT_MAX, maxX = 0;

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
                raster_point v1 = vertices[i];
                raster_point v2 = vertices[(i + 1) % numVertices];
                if ((scanY >= v1.y && scanY < v2.y) || (scanY >= v2.y && scanY < v1.y))  // v1 v2 不在同一行
                {
                    float slope = (v2.x - v1.x) / (v2.y - v1.y);
                    float x = (scanY - v1.y) * slope + v1.x;
                    edges.push_back(static_cast<int>(x));
                }
                else if (v1.y == v2.y && scanY == v1.y)  // v1 v2 在同一行
                {
                    edges.push_back(v1.x);
                    edges.push_back(v2.x);
                }
            }
        }
        // 根据X值对边缘进行排序
        std::sort(edges.begin(), edges.end());
        for (int i = 0; i < edges.size(); i += 2)
        {
            cover_pair tmp_pair;
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
    png_data png = png_utils::create_empty(nrows, ncols, PNG_COLOR_TYPE_RGB_ALPHA);

    for (int r = 0; r < nrows; ++r)
    {
        int old = 0;
        for (auto p : row_pairs[r])
        {
            for (int i = p.beg; i < p.end; ++i)
            {
                png.set_pixel(r, i, silly_color(255, 255, 0, 175));
            }
            old = p.end;
        }
    }

    png_utils::write(path.c_str(), png);
    png.release();
#endif
}
