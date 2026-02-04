//
// Created by dell on 2023/12/6.
//

#include "silly_vector_to_raster.h"

void XScanLine::check_line_point(const suPoint&  point, std::vector<Point>& vct, int& last_x, int& last_y) const
{
    // m_row_pairs.clear();
    int tmp_x = static_cast<int>(std::round((point.x - m_rect.min.x) / m_cell_size));
    int tmp_y = static_cast<int>(std::round((m_rect.max.y - point.y) / m_cell_size));
    if (last_x != tmp_x || last_y != tmp_y)
    {
        vct.push_back(Point(tmp_x, tmp_y));
        last_x = tmp_x;
        last_y = tmp_y;
    }
}

void XScanLine::rasterize(const suPoint& point)
{
    int x = static_cast<int>(std::round((point.x - m_rect.min.x) / m_cell_size));
    int y = static_cast<int>(std::round((m_rect.max.y - point.y) / m_cell_size));
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_row_colors[y][x] = 1;
    }
}

void XScanLine::rasterize(const suMultiPoint& points)
{
    // m_row_pairs.clear();
    for (const auto& point : points)
    {
        rasterize(point);
    }
}

void XScanLine::rasterize(const suLine& line)
{
    if (line.empty())
    {
        return;
    }
    size_t i = 0;

    int bx = static_cast<int>(std::round((line[i].x - m_rect.min.x) / m_cell_size));
    int by = static_cast<int>(std::round((m_rect.max.y - line[i].y) / m_cell_size));
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

void XScanLine::rasterize(const suMultiLine& lines)
{
    // m_row_pairs.clear();
    for (const auto& line : lines)
    {
        rasterize(line);
    }
}

void XScanLine::rasterize(const suPoly& poly)
{
    // m_row_pairs.clear();
    std::vector<std::vector<Point>> vertices_arr;

    // 将点转换为光栅坐标并合并连续的相同点
    int last_x = 0 - m_width;
    int last_y = 0 - m_height;
    std::vector<Point> tmp_vertices;

    for (const auto& point : poly.outer.points)
    {
        check_line_point(point, tmp_vertices, last_x, last_y);
    }
    vertices_arr.push_back(tmp_vertices);

    for (const auto& ring : poly.holes)
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

void XScanLine::rasterize(const suMultiPoly& m_polys)
{
    // m_row_pairs.clear();
    std::vector<std::vector<Point>> vertices_arr;

    for (const auto& poly : m_polys)
    {
        // 由于精度(m_cell_size)问题,可能会导致一个矢量中有大量重复的连续栅格点,将这部分连续栅格点归并为一个
        int last_x = 0 - m_width;
        int last_y = 0 - m_height;
        for (const auto& ring : poly.holes)
        {
            std::vector<Point> tmp_vertices;
            for (const auto& point : ring.points)
            {
                check_line_point(point, tmp_vertices, last_x, last_y);
            }
            vertices_arr.push_back(tmp_vertices);
        }
        std::vector<Point> tmp_vertices;

        for (const auto& point : poly.outer.points)
        {
            check_line_point(point, tmp_vertices, last_x, last_y);
        }
        vertices_arr.push_back(tmp_vertices);
    }

    rasterize(vertices_arr);
}

void XScanLine::rasterize(const std::vector<std::vector<Point>>& vertices_arr)
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
    minY = SU_MIN(minY, m_height - 1);
    maxY = SU_MIN(maxY, m_height - 1);
    minX = SU_MIN(minX, m_width - 1);
    maxX = SU_MIN(maxX, m_width - 1);

    // 对每一条扫描线进行处理
    for (int scanY = minY; scanY <= maxY; ++scanY)
    {
        // 构建边缘列表
        std::vector<int> edges;
        for (size_t part = 0; part < vertices_arr.size(); ++part)
        {
            auto vertices = vertices_arr[part];
            size_t numVertices = vertices.size();
            for (int i = 0; i < numVertices; ++i)
            {
                Point v1 = vertices[i];
                Point v2 = vertices[(i + 1) % numVertices];
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

void XScanLine::rasterize(const suGeoColl& gc)
{
    switch (gc.type())
    {
        case eGeometryType::Point:  // 单点
            rasterize(gc.asPoint());
            break;
        case eGeometryType::LineString:  // 单线
            rasterize(gc.asLine());
            break;
        case eGeometryType::Polygon:  // 单面
            rasterize(gc.asPoly());
            break;
        case eGeometryType::MultiPoint:  // 多点
            rasterize(gc.asMultiPoint());
            break;
        case eGeometryType::MultiLineString:  // 多线
            rasterize(gc.asMultiLine());
            break;
        case eGeometryType::MultiPolygon:  // 多面
            rasterize(gc.asMultiPoly());
            break;
        default:
            std::cerr << "无效类型" << std::endl;
            break;
    }
}

#ifndef NDEBUG
#include <graphics/silly_png.h>
#endif

void XScanLine::image(const suPath& file)
{
#ifndef NDEBUG
    suPNG pd;
    pd.create(m_width, m_height, eColorType::GRAYA);
    for (int r = 0; r < m_height; ++r)
    {
        int old = 0;
        for (auto p : m_row_pairs[r])
        {
            for (int i = p.beg; i <= p.end; ++i)
            {
                pd.pixel(r, i, suColor(255, 0, 0, 255));
            }
            old = p.end;
        }
    }

    pd.write(file);
    pd.release();
#endif
}
void XScanLine::set(const suRect& rect, const double& cell_size)
{
    clear();
    m_rect = rect;
    m_rect.correct();
    m_cell_size = cell_size;
    m_width = static_cast<int>(std::round((m_rect.max.x - m_rect.min.x) / m_cell_size));
    m_height = static_cast<int>(std::round((m_rect.max.y - m_rect.min.y) / m_cell_size));
    m_row_colors.resize(m_height, std::vector<uint8_t>(m_width, 0x00));
}

int XScanLine::width() const
{
    return m_width;
}

int XScanLine::height() const
{
    return m_height;
}

XScanLine::ScanPairs XScanLine::row_pairs() const
{
    return m_row_pairs;
}

void XScanLine::clear()
{
    m_row_pairs.clear();
    m_row_colors.clear();
}

int64_t XScanLine::num() const
{
    return m_num;
}
suMatrix<uint8_t> XScanLine::mask()
{
    suMatrix<uint8_t> ret;
    /*int row = std::round((m_rect.max.y - m_rect.min.y) / m_cell_size);
    int col = std::round((m_rect.max.x - m_rect.min.x) / m_cell_size);*/
    ret.create(m_height, m_width);
    ret.set(0);
    uint8_t fv = 1;
    for (auto& [r, bePairs] : m_row_pairs)
    {
        uint8_t* p = ret[r];
        for (const auto& [beg, end] : bePairs)
        {
            for (int c = beg; c <= end; ++c)
            {
                memcpy(p + c, &fv, sizeof(uint8_t) * (end - beg + 1));
            }
        }
    }
    return ret;
}

std::vector<suPoly> XScanLine::grids() const
{
    std::vector<suPoly> ret;
    for (auto& [r, b_es] : m_row_pairs)
    {
        for (const auto& [beg, end] : b_es)
        {
            for (int c = beg; c <= end; ++c)
            {
                double lon = m_rect.min.x + c * m_cell_size;
                double lat = m_rect.max.y - r * m_cell_size;
                suPoly poly;
                poly.outer.points.push_back({lon, lat});
                poly.outer.points.push_back({lon + m_cell_size, lat});
                poly.outer.points.push_back({lon + m_cell_size, lat - m_cell_size});
                poly.outer.points.push_back({lon, lat - m_cell_size});
                poly.outer.points.push_back({lon, lat});

                ret.push_back({poly});
            }
        }
    }

    return ret;
}
void XScanLine::add(const int& row, const std::vector<int>& edges)
{
    if (edges.size() % 2 == 1 || edges.empty() || row >= m_height)
    {
        // std::cerr << "无法添加" << std::endl;
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
void XScanLine::fill()
{
    m_row_pairs.clear();
    m_num = 0;
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
                m_num += (i - 1 - b);
                continue;
            }
        }
    }
}
