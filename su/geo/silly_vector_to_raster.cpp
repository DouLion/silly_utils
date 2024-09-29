//
// Created by dell on 2023/12/6.
//

#include "silly_vector_to_raster.h"

void xscan_line_raster::check_line_point(silly_point point, std::vector<raster_point>& vct, int& last_x, int& last_y)
{
    // m_row_pairs.clear();
    int tmp_x = std::round((point.x - m_rect.min.x) / m_cell_size);
    int tmp_y = std::round((m_rect.max.y - point.y) / m_cell_size);
    if (last_x != tmp_x || last_y != tmp_y)
    {
        vct.push_back(raster_point(tmp_x, tmp_y));
        last_x = tmp_x;
        last_y = tmp_y;
    }
}

bool xscan_line_raster::rasterization(const silly_point& point)
{
    int raster_x = std::round((point.x - m_rect.min.x) / m_cell_size);
    int raster_y = std::round((m_rect.max.y - point.y) / m_cell_size);
    if (raster_x >= 0 && raster_x <= m_width && raster_y >= 0 && raster_y <= m_height)
    {
        m_row_pairs[raster_y].push_back({raster_x, raster_x});
        return true;
    }
    return false;
}

bool xscan_line_raster::rasterization(const silly_multi_point& points)
{
    // m_row_pairs.clear();
    for (const auto& point : points)
    {
        rasterization(point);
    }
    slim();
    return true;
}

bool xscan_line_raster::rasterization(const silly_line& line)
{
    // m_row_pairs.clear();
    if (line.size() < 2)
    {
        return false;  // 如果线段小于两个点，则无法处理
    }
    // 遍历线段中的每一对点
    std::map<int, std::map<int, int>> row_col;
    int last_x = std::round((line.front().x - m_rect.min.x) / m_cell_size);
    int last_y = std::round((m_rect.max.y - line.front().y) / m_cell_size);
    for (auto& point : line)
    {
        int x = std::round((point.x - m_rect.min.x) / m_cell_size);
        int y = std::round((m_rect.max.y - point.y) / m_cell_size);
        if (x != last_x || y != last_y)
        {
            row_col[y][x] = 0;
        }
        else
        {
            continue;
        }

        if (x = last_x)
        {
            for (int ny = std::min(last_y, y); ny < std::max(last_y, y); ny++)
            {
                row_col[ny][x] = 0;
            }
        }
        last_x = x;
        last_y = y;
    }

    slim();
    return true;
}

bool xscan_line_raster::rasterization(const silly_multi_silly_line& lines)
{
    // m_row_pairs.clear();
    for (const auto& line : lines)
    {
        rasterization(line);
    }
    slim();

    return true;
}

bool xscan_line_raster::rasterization(const silly_poly& poly)
{
    // m_row_pairs.clear();
    std::vector<std::vector<raster_point>> vertices_arr;

    // 将点转换为光栅坐标并合并连续的相同点
    int last_x = 0 - m_width;
    int last_y = 0 - m_height;
    std::vector<raster_point> tmp_vertices;

    for (const auto& point : poly.outer_ring.points)
    {
        check_line_point(point, tmp_vertices, last_x, last_y);
    }
    vertices_arr.push_back(tmp_vertices);

    for (const auto& ring : poly.inner_rings)
    {
        tmp_vertices.clear();
        last_x = 0 - m_width;
        last_y = 0 - m_height;
        for (const auto& point : ring.points)
        {
            check_line_point(point, tmp_vertices, last_x, last_y);
        }
        vertices_arr.push_back(tmp_vertices);
    }

    return rasterization(vertices_arr);
}

bool xscan_line_raster::rasterization(const silly_multi_poly& m_polys)
{
    // m_row_pairs.clear();
    std::vector<std::vector<raster_point>> vertices_arr;

    for (const auto& poly : m_polys)
    {
        // 由于精度(m_cell_size)问题,可能会导致一个矢量中有大量重复的连续栅格点,将这部分连续栅格点归并为一个
        int last_x = 0 - m_width;
        int last_y = 0 - m_height;
        for (const auto& ring : poly.inner_rings)
        {
            std::vector<raster_point> tmp_vertices;
            for (const auto& point : ring.points)
            {
                check_line_point(point, tmp_vertices, last_x, last_y);
            }
            vertices_arr.push_back(tmp_vertices);
        }
        std::vector<raster_point> tmp_vertices;

        for (const auto& point : poly.outer_ring.points)
        {
            check_line_point(point, tmp_vertices, last_x, last_y);
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
            m_row_pairs[scanY].push_back(tmp_pair);
        }
    }
    return true;
}

bool xscan_line_raster::rasterization(const silly_geo_coll& geo_coll)
{
    bool status = false;
    enum_geometry_type feature_type = geo_coll.m_type;
    switch (feature_type)
    {
        case enum_geometry_type::egtPoint:  // 单点
            status = rasterization(geo_coll.m_point);
            break;
        case enum_geometry_type::egtLineString:  // 单线
            status = rasterization(geo_coll.m_line);
            break;
        case enum_geometry_type::egtPolygon:  // 单面
            status = rasterization(geo_coll.m_poly);
            break;
        case enum_geometry_type::egtMultiPoint:  // 多点
            status = rasterization(geo_coll.m_m_points);
            break;
        case enum_geometry_type::egtMultiLineString:  // 多线
            status = rasterization(geo_coll.m_m_lines);
            break;
        case enum_geometry_type::egtMultiPolygon:  // 多面
            status = rasterization(geo_coll.m_m_polys);
        default:
            SU_ERROR_PRINT("Unprocessable data types: %d\n", feature_type);
            break;
    }

    return status;
}

#ifndef NDEBUG
#include <image/png_utils.h>
using namespace silly_image;
#endif

void xscan_line_raster::image(const std::string& path)
{
#ifndef NDEBUG
    png_data png = png_utils::create_empty(m_height, m_width, PNG_COLOR_TYPE_RGB_ALPHA);
    // #pragma omp parallel for num_threads(8)
    for (int r = 0; r < m_height; ++r)
    {
        int old = 0;
        for (auto p : m_row_pairs[r])
        {
            for (int i = p.beg; i <= p.end; ++i)
            {
                png.set_pixel(r, i, silly_color(255, 0, 0, 255));
            }
            old = p.end;
        }
    }

    png_utils::write(path.c_str(), png);
    png.release();
#endif
}
void xscan_line_raster::set(const silly_geo_rect& rect, const double& cell_size)
{
    m_rect = rect;
    m_rect.correct();
    m_cell_size = cell_size;
    m_width = static_cast<int>(std::ceil((m_rect.max.x - m_rect.min.x) / m_cell_size));
    m_height = static_cast<int>(std::ceil((m_rect.max.y - m_rect.min.y) / m_cell_size));
}
int xscan_line_raster::width() const
{
    return m_width;
}
int xscan_line_raster::height() const
{
    return m_height;
}
std::map<int, std::vector<cover_pair>> xscan_line_raster::row_pairs() const
{
    return m_row_pairs;
}
void xscan_line_raster::slim()
{
    std::map<int, std::map<int, int>> r_c_i;
    for (auto [r, cs] : m_row_pairs)
    {
        if (r < 0 || r >= m_height)
        {
            continue;
        }
        for (auto c : cs)
        {
            if (c.beg < 0 || c.beg >= m_width)
            {
                continue;
            }

            r_c_i[r][c.beg] = 0;
        }
    }
    m_row_pairs.clear();
    for (auto [r, c_i] : r_c_i)
    {
        for (auto [c, i] : c_i)
        {
            m_row_pairs[r].push_back({c, c});
        }
    }
}
