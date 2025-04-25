//
// Created by dell on 2023/12/6.
//

#include "silly_vector_to_raster.h"
using namespace silly::geo::rasterization;

void x_scan_line::check_line_point(silly_point point, std::vector<_point>& vct, int& last_x, int& last_y)
{
    // m_row_pairs.clear();
    int tmp_x = static_cast<int>((point.x - m_rect.min.x) / m_cell_size);
    int tmp_y = static_cast<int>((m_rect.max.y - point.y) / m_cell_size);
    if (last_x != tmp_x || last_y != tmp_y)
    {
        vct.push_back(_point(tmp_x, tmp_y));
        last_x = tmp_x;
        last_y = tmp_y;
    }
}

void x_scan_line::rasterize(const silly_point& point)
{
    int x = static_cast<int>((point.x - m_rect.min.x) / m_cell_size);
    int y = static_cast<int>((m_rect.max.y - point.y) / m_cell_size);
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_row_colors[y][x] = 1;
    }
}

void x_scan_line::rasterize(const silly_multi_point& points)
{
    // m_row_pairs.clear();
    for (const auto& point : points)
    {
        rasterize(point);
    }
}

void x_scan_line::rasterize(const silly_line& line)
{
    if (line.empty())
    {
        return;
    }
    int i = 0;

    int bx = static_cast<int>((line[i].x - m_rect.min.x) / m_cell_size);
    int by = static_cast<int>((m_rect.max.y - line[i].y) / m_cell_size);
    double dbx = line[i].x;
    double dby = line[i].y;
    for (i = 1; i < line.size(); i++)
    {
       double dex = line[i].x;
       double dey = line[i].y;
       // TODO: 后续再补充. 在同一行可能会占用两个格子
       std::cerr << "TODO: 未完成" << std::endl;
       return;
    }
    
}

void x_scan_line::rasterize(const silly_multi_line& lines)
{
    // m_row_pairs.clear();
    for (const auto& line : lines)
    {
        rasterize(line);
    }
}

void x_scan_line::rasterize(const silly_poly& poly)
{
    // m_row_pairs.clear();
    std::vector<std::vector<_point>> vertices_arr;

    // 将点转换为光栅坐标并合并连续的相同点
    int last_x = 0 - m_width;
    int last_y = 0 - m_height;
    std::vector<_point> tmp_vertices;

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

    rasterize(vertices_arr);
}

void x_scan_line::rasterize(const silly_multi_poly& m_polys)
{
    // m_row_pairs.clear();
    std::vector<std::vector<_point>> vertices_arr;

    for (const auto& poly : m_polys)
    {
        // 由于精度(m_cell_size)问题,可能会导致一个矢量中有大量重复的连续栅格点,将这部分连续栅格点归并为一个
        int last_x = 0 - m_width;
        int last_y = 0 - m_height;
        for (const auto& ring : poly.inner_rings)
        {
            std::vector<_point> tmp_vertices;
            for (const auto& point : ring.points)
            {
                check_line_point(point, tmp_vertices, last_x, last_y);
            }
            vertices_arr.push_back(tmp_vertices);
        }
        std::vector<_point> tmp_vertices;

        for (const auto& point : poly.outer_ring.points)
        {
            check_line_point(point, tmp_vertices, last_x, last_y);
        }
        vertices_arr.push_back(tmp_vertices);
    }

    rasterize(vertices_arr);
}

void x_scan_line::rasterize(const std::vector<std::vector<_point>> vertices_arr)
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
                _point v1 = vertices[i];
                _point v2 = vertices[(i + 1) % numVertices];
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
        add(scanY, edges);
    }
}

void x_scan_line::rasterize(const silly_geo_coll& geo_coll)
{
    enum_geometry_type feature_type = geo_coll.m_type;
    switch (feature_type)
    {
        case enum_geometry_type::egtPoint:  // 单点
            rasterize(geo_coll.m_point);
            break;
        case enum_geometry_type::egtLineString:  // 单线
            rasterize(geo_coll.m_line);
            break;
        case enum_geometry_type::egtPolygon:  // 单面
            rasterize(geo_coll.m_poly);
            break;
        case enum_geometry_type::egtMultiPoint:  // 多点
            rasterize(geo_coll.m_m_points);
            break;
        case enum_geometry_type::egtMultiLineString:  // 多线
            rasterize(geo_coll.m_m_lines);
            break;
        case enum_geometry_type::egtMultiPolygon:  // 多面
            rasterize(geo_coll.m_m_polys);
            break;
        default:
            std::cerr <<  "无效类型" << std::endl;
            break;
    }
}

#ifndef NDEBUG
#include <graphics/silly_png.h>
#endif

void x_scan_line::image(const std::string& path)
{
#ifndef NDEBUG
    silly::png::data pd;
    pd.create(m_height, m_width, silly::color::type::eptGRAYA);
    for (int r = 0; r < m_height; ++r)
    {
        int old = 0;
        for (auto p : m_row_pairs[r])
        {
            for (int i = p.beg; i <= p.end; ++i)
            {
                pd.pixel(r, i, silly::color(255, 0, 0, 255));
            }
            old = p.end;
        }
    }

    pd.write(path);
    pd.release();
#endif
}
void x_scan_line::set(const silly_geo_rect& rect, const double& cell_size)
{
    clear();
    m_rect = rect;
    m_rect.correct();
    m_cell_size = cell_size;
    m_width = static_cast<int>(std::ceil((m_rect.max.x - m_rect.min.x) / m_cell_size));
    m_height = static_cast<int>(std::ceil((m_rect.max.y - m_rect.min.y) / m_cell_size));
    m_row_colors.resize(m_height, std::vector<uint8_t>(m_width, 0x00));
}
int x_scan_line::width() const
{
    return m_width;
}
int x_scan_line::height() const
{
    return m_height;
}
scan_pairs x_scan_line::row_pairs() const
{
    return m_row_pairs;
}

void x_scan_line::clear()
{
    m_row_pairs.clear();
    m_row_colors.clear();
}

std::vector<silly_poly> x_scan_line::grids() const
{
    std::vector<silly_poly> ret;
    for (auto [r, b_es] : m_row_pairs)
    {
        for (auto b_e : b_es)
        {
            for (int c = b_e.beg; c <= b_e.end; ++c)
            {
                
                double lon = m_rect.min.x + c * m_cell_size;
                double lat = m_rect.max.y - r * m_cell_size;
                silly_poly poly;
                poly.outer_ring.points.push_back({lon, lat});
                poly.outer_ring.points.push_back({lon + m_cell_size, lat});
                poly.outer_ring.points.push_back({lon + m_cell_size, lat - m_cell_size});
                poly.outer_ring.points.push_back({lon, lat - m_cell_size});
                poly.outer_ring.points.push_back({lon, lat});

                ret.push_back({poly});
            }
        }
    }

    return ret;
}
void x_scan_line::add(const int& row, const std::vector<int>& edges)
{
    if (edges.size() % 2 == 1)
    {
        std::cerr << "无法添加" << std::endl;
        return;
    }
    std::vector<uint8_t>& tmp_rc = m_row_colors[row];
    for (int i = 0; i <= edges.size() - 1; i += 2)
    {
        int b0 = std::min(m_width - 1, std::max(edges[i], 0));
        int e0 = std::min(m_width - 1, std::max(edges[i + 1], 0));
        for (int j = b0; j <= e0; ++j)
        {
            tmp_rc[j] = 0x01;
        }
    }
}
void x_scan_line::fill()
{
    m_row_pairs.clear();
    for (int r = 0; r < m_row_colors.size(); ++r)
    {
        auto& cs = m_row_colors[r];
        for (int i = 0; i < cs.size(); ++i)
        {
            if (cs[i] == 0x01)
            {
                int b = i;
                for (; i < cs.size(); ++i)
                {
                    if (cs[i] == 0x00 || i == cs.size() - 1)
                    {
                        m_row_pairs[r].push_back({b, i - 1});
                        break;
                    }
                }
                continue;
            }
        }
    }
}
