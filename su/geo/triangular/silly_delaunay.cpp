//
// Created by dell on 2024/1/24.
//

#include "silly_delaunay.h"

void silly_dt_tri::calc_circle()
{
    if (sq_radius > SU_EPSILON)  // 认为已经计算过
    {
        return;
    }

    // 圆心坐标公式
    // x = ((C1*B2)-(C2*B1))/((A1*B2)-(A2*B1))
    // y = ((A1*C2)-(A2*C1))/((A1*B2)-(A2*B1))

    const auto ax = p1.x - p0.x;
    const auto ay = p1.y - p0.y;
    const auto bx = p2.x - p0.x;
    const auto by = p2.y - p0.y;

    // 检查分母是否接近于零
    const double denominator = ax * by - ay * bx;
    if (fabs(denominator) < SU_EPSILON)
    {
        sq_radius = 0;
        return;
    }

    const auto m = p1.x * p1.x - p0.x * p0.x + p1.y * p1.y - p0.y * p0.y;
    const auto u = p2.x * p2.x - p0.x * p0.x + p2.y * p2.y - p0.y * p0.y;
    const auto s = 1. / (2. * denominator);

    center.x = ((p2.y - p0.y) * m + (p0.y - p1.y) * u) * s;
    center.y = ((p0.x - p2.x) * m + (p1.x - p0.x) * u) * s;

    const auto dx = p0.x - center.x;
    const auto dy = p0.y - center.y;
    sq_radius = dx * dx + dy * dy;
}
bool silly_dt_tri::in_circle(const silly_dt_point& p) const
{
    return silly_delaunay_utils::sq_dist(p, center) < sq_radius;
}

double silly_delaunay_utils::get_double(const std::string& str, const size_t& limit, size_t& i)
{
    char d[100] = {0};

    int z = 0;
    while (i < limit && str[i] != ',')
    {
        if (str[i] == '\n')
            break;
        d[z] = str[i];
        i++;
        z++;
    }
    i++;
    return std::stod(d);
}

std::vector<silly_dt_point> silly_delaunay_utils::read(const std::string& file)
{
    std::vector<silly_dt_point> points;
    std::string content;
    if (!silly_file::read(file, content))
    {
        return points;
    }
    return parse(content);
}

std::vector<silly_dt_point> silly_delaunay_utils::parse(const std::string& content)
{
    std::vector<silly_dt_point> points;
    std::string ncontent = content;
    ncontent.append("\n");
    size_t size = ncontent.size();
    size_t i = 0;
    while (i < size)
    {
        silly_dt_point tmp;
        tmp.x = get_double(content, size, i);
        tmp.y = get_double(content, size, i);
        tmp.val = get_double(content, size, i);
        tmp.index = static_cast<uint32_t>(i);
        points.push_back(tmp);
    }
    return points;
}

double silly_delaunay_utils::dist(const silly_dt_point& p0, const silly_dt_point& p1)
{
    return sqrt(sq_dist(p0, p1));
}

double silly_delaunay_utils::sq_dist(const silly_dt_point& p0, const silly_dt_point& p1)
{
    return (p0.x - p1.x) * (p0.x - p1.x) + (p0.y - p1.y) * (p0.y - p1.y);
}
#ifndef NDEBUG
#include <graphics/render/canvas/silly_cairo.h>
void silly_delaunay::draw(const std::string& path)
{
    silly_cairo sc;
    sc.create(2000, 2000);
    silly_geo_rect rect{xmin, ymax, xmax, ymin};
    rect.correct();
    sc.set({0, 180, 255, 230});
    for (auto tri : m_tris)
    {
        sc.draw_line({tri.p0, tri.p1}, rect);
        sc.draw_line({tri.p1, tri.p2}, rect);
        sc.draw_line({tri.p2, tri.p0}, rect);
    }
    sc.write(path);
    sc.release();
}
#else
void silly_delaunay::draw(const std::string& path)
{
}
#endif
