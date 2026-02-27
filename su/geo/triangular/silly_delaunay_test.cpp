//
// Created by dell on 2024/1/24.
//

#include "silly_delaunay.h"
#ifndef NDEBUG
#include <datetime/silly_timer.h>
#include <log/silly_log.h>

#include <graphics/render/canvas/silly_cairo.h>
#include <database/otl/silly_otl.h>
std::vector<double> thresholds = {0.1, 10, 25, 50, 100, 150, 250};
std::vector<std::string> colors = {"afc9ecc0", "afa9dd9b", "af7bcc7e", "af009989", "af003a8f", "af00006e", "af00004b"};
void suDelaunay::Test2()
{
    double width = 2000;
    double height = 2000;
    std::string content = sufile::read("Z:/fore_iso.txt");
    std::vector<suPoint> pts;
    {
        // 创建字符串流用于分割
        std::istringstream ss(content);
        std::string token;
        std::vector<double> values;

        // 分割字符串并转换为double
        while (std::getline(ss, token, ','))
        {
            values.push_back(std::stod(token));
        }

        // 检查数据量是否完整（应为3的倍数）
        if (values.size() % 3 != 0)
        {
            SLOG_ERROR("数据格式错误：元素数量不是3的倍数")
            return;
        }
        suRect bound = {108, 32, 115, 23.5};
        double bdw = bound.max.x - bound.min.x;
        double bdh = bound.max.y - bound.min.y;

        // 每三个元素组成一个元组
        for (size_t i = 0; i < values.size(); i += 3)
        {
            double x = values[i];
            double y = values[i + 1];
            double v = values[i + 2];
            x = (x - bound.min.x) / bdw * width;
            y = (bound.max.y - y) / bdh * height;
            pts.emplace_back(x, y, 0, values[i + 2]);
        }
    }

    suTimer timer;
    BuildTriangles(pts);
    SLOG_DEBUG("点数量: {}, 时间: {} ms", m_points.size(), timer.elapsed_ms())
    SLOG_DEBUG("三角形数量: {}", m_tris.size());
    suCairo cairo;
    cairo.create(width, height);
    cairo.set(CAIRO_OPERATOR_SOURCE);
    if (0)
    {  // 三角形
        cairo.set(suColor(255, 51, 153, 255));
        for (const auto& [a, b, c] : m_tris)
        {
            cairo.draw_line({m_points[a], m_points[b], m_points[c], m_points[a]});
        }
    }
    for (int i = 0; i < thresholds.size(); ++i)
    {
        double th = thresholds[i] - 0.000001;
        suColor color;
        color.hex2argb(colors[i]);
        cairo.set(color);
        std::vector<suPoly> polys;
        timer.restart();
        TracePoly(th, polys);
        SLOG_DEBUG("面数量: {}, 时间: {} ms", polys.size(), timer.elapsed_ms())

        cairo.draw_poly(polys);
    }
    
    if (1)
    {
        for (int i = 0; i < thresholds.size(); ++i)
        {
            double th = thresholds[i] - 0.000001;
            suColor color;
            color.hex2argb(colors[i]);
            color.alpha = 255;
            cairo.set(color);
            for (const auto& p : m_points)
            {
                if (p.v >= th)
                {
                    cairo.draw_point(p, 3); 
                }
            }
        }
    }

    cairo.write("./all.png");
    cairo.release();
}
void suDelaunay::TestWithDB()
{
    std::string sql = "select b.LGTD, b.LTTD, SUM(DRP) as total from ST_PPTN_R a LEFT JOIN ST_STBPRP_B b on a.STCD = b.STCD where TM > '2024-06-24 08:00' and TM < '2024-06-25 08:00' and DRP > 0 GROUP BY a.STCD order  by total desc";
    std::string db = R"(
{
  "type": "mysql",
  "ip": "192.168.0.73",
  "port": 3306,
  "driver": "MariaDB ODBC 3.1 Driver",
  "schema": "RWDB_HN",
  "user": "root",
  "password": "3edc9ijn~"
}
)";

    suOTL otl;
    otl.from_json(db);
    otl.check();
    std::vector<suPoint> points;
    if (!otl.select(sql, [&points](otl_stream* stm) {
            while (!stm->eof())
            {
                std::string stcd;
                double lgtd, lttd, total;
                otl_read_row(*stm, lgtd, lttd, total);
                points.push_back({lgtd, lttd, 0, total});
            }
        }))
    {
        SLOG_ERROR(otl.err())
        return;
    }
    suTimer timer;
    BuildTriangles(points);
    suRect bound;
    bound.min = {108.5, 24.5};
    bound.max = {114.5, 30.5};
    int width = 2000;
    int height = 2000;
    suCairo cairo;
    cairo.create(width, height);
    for (int i = 0; i < thresholds.size(); ++i)
    {
        double th = thresholds[i];
        suColor color;
        color.hex2argb(colors[i]);
        std::vector<suPoly> polys;
        TracePoly(th, polys);
        for (auto& poly : polys)
        {
            poly.outer.points = SU_SMOOTH_B_SPLINE(poly.outer.points, 10);
            for (auto& ring : poly.holes)
            {
                ring.points = SU_SMOOTH_B_SPLINE(ring.points, 10);
            }
        }
        cairo.set(color);
        for (const auto& poly : polys)
        {
            cairo.draw_poly(poly, bound);
        }
    }
    SLOG_INFO("{}ms", timer.elapsed_ms())
    cairo.write("./iso_db.png");
    cairo.release();
}

void suDelaunay::Test()
{
    double width = 2000;
    double height = 2000;
    std::vector<suPoint> pts;
    int num = 1000;
    std::vector<double> vs = RANDOM(0.1 * width, 0.9 * width, num * 2);
    std::vector<double> dd = RANDOM(10, 120, num);
    suPoint p0(width / 2, height / 2);
    for (int i = 0; i < num; i++)
    {
        suPoint tmp(vs[i * 2], vs[i * 2 + 1]);
        // tmp.v = tmp.dist(p0);
        tmp.v = (int)dd[i];
        pts.push_back(tmp);
    }
    suTimer timer;
    BuildTriangles(pts);
    SLOG_DEBUG("点数量: {}, 时间: {} ms", m_points.size(), timer.elapsed_ms())
    SLOG_DEBUG("三角形数量: {}", m_tris.size());

    double th = 60.1;
    // auto tris = delaunay.triangles();
    {  // 画三角
        suCairo cairo;
        cairo.create(width, height);
        cairo.set(suColor(0, 0, 255, 255));
        suRect nr;
        nr.min = suPoint(0, 0);
        nr.max = suPoint(width, height);
        for (const auto& [a, b, c] : m_tris)
        {
            cairo.draw_line({m_points[a], m_points[b], m_points[c], m_points[a]});
        }
        cairo.set(suColor(255, 0, 0, 255));
        for (const auto& p : m_points)
        {
            cairo.draw_point(p, 4);
        }
        cairo.write("./tri.png");
    }
    if (0)
    {
        std::vector<suLine> lines;

        timer.restart();
        TraceLine(th, lines);
        SLOG_DEBUG("线数量: {}, 时间: {} ms", lines.size(), timer.elapsed_ms())
        // 画线
        suCairo cairo;
        cairo.create(width, height);
        cairo.set(suColor(0, 0, 255, 255));

        for (const auto& line : lines)
        {
            cairo.draw_line(line.to_vector());
        }
        cairo.set(suColor(255, 0, 0, 255));
        for (const auto& p : m_points)
        {
            cairo.draw_point(p, 4);
        }
        cairo.write("./isoline.png");
    }

    if (1)
    {
        std::vector<suPoly> polys;
        timer.restart();
        TracePoly(th, polys);
        SLOG_DEBUG("面数量: {}, 时间: {} ms", polys.size(), timer.elapsed_ms())
        // 画线
        suCairo cairo;
        cairo.create(width, height);
        cairo.set(suColor(0, 0, 255, 255));

        for (const auto& poly : polys)
        {
            cairo.draw_poly(poly);
        }

        for (const auto& p : m_points)
        {
            if (p.v >= th)
            {
                cairo.set(suColor(255, 0, 0, 255));
            }
            else
            {
                cairo.set(suColor(0, 255, 0, 255));
            }
            cairo.draw_point(p, 6);
        }
        cairo.write("./isoarea.png");
    }
}
#endif