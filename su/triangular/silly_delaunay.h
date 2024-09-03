/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/24 11:53
 * @version: 1.0.1
 * @description: 德劳内 Bowyer-Watson algorithm C++ implementation of http://paulbourke.net/papers/triangulate .
 */
#ifndef SILLY_UTILS_SILLY_DELAUNAY_H
#define SILLY_UTILS_SILLY_DELAUNAY_H

#include <geo/silly_geo.h>
#include <files/silly_file.h>

class silly_dt_point : public silly_point
{
  public:
    silly_dt_point() = default;

    silly_dt_point(double x, double y)
    {
        lgtd = x;
        lttd = y;
    }
    silly_dt_point(double x, double y, double v)
    {
        lgtd = x;
        lttd = y;
        val = v;
    }

  public:
    double val{0.};
};

/*namespace std
{
template <>
struct hash<silly_dt_point>
{
    size_t operator()(const silly_dt_point& point) const
    {
        return hash<double>()(point.lgtd) ^ hash<double>()(point.lttd);
    }
};
}  // namespace std
*/

class silly_dt_edge
{
  public:
    silly_dt_edge() = default;
    silly_dt_edge(silly_dt_point _p0, silly_dt_point _p1) : p0(_p0), p1(_p1)
    {
    }
    silly_dt_edge operator=(const silly_dt_edge& e)
    {
        p0 = e.p0;
        p1 = e.p1;
        return *this;
    }
    bool operator==(const silly_dt_edge& e) const
    {
        return (p0 == e.p0 && p1 == e.p1) || (p0 == e.p1 && p1 == e.p0);
    }

    silly_dt_point p0, p1;
};

// 自定义哈希函数
/*
namespace std
{
template <>
struct hash<silly_dt_edge>
{
    size_t operator()(const silly_dt_edge& edge) const
    {
        // 使用点的哈希值组合
        return hash<silly_dt_point>()(edge.p0) ^ hash<silly_dt_point>()(edge.p1);
    }
};
}  // namespace std
*/

class silly_dt_tri
{
  public:
    silly_dt_tri(silly_dt_point _p0, silly_dt_point _p1, silly_dt_point _p2)
    {
        p0 = _p0;
        p1 = _p1;
        p2 = _p2;
        /*        e0 = silly_dt_edge{_p0, _p1};
                e1 = silly_dt_edge{_p1, _p2};
                e2 = silly_dt_edge{_p2, _p0};*/
    }
    void calc_circle();
    bool in_circle(const silly_dt_point& p) const;

  public:
    silly_dt_point p0, p1, p2;
    // silly_dt_edge e0, e1, e2;
    double sq_radius{0};    // 半径平方
    silly_dt_point center;  // 外接圆心
};



class silly_delaunay_utils
{
  public:
    static double get_double(const std::string& str, const size_t& limit, size_t& i);
    static std::vector<silly_dt_point> read(const std::string& file);
    static std::vector<silly_dt_point> parse(const std::string& content);
    static double dist(const silly_dt_point& p0, const silly_dt_point& p1);
    static double sq_dist(const silly_dt_point& p0, const silly_dt_point& p1);
};

class silly_delaunay
{
  public:
    virtual void points(std::vector<silly_dt_point>& pts) = 0;
    virtual void triangulate() = 0;
    void draw(const std::string& path);

  protected:
    std::vector<silly_dt_point> m_points{};
    std::vector<silly_dt_tri> m_tris{};
    double xmin = std::numeric_limits<double>::max();
    double ymin = std::numeric_limits<double>::max();
    double xmax = std::numeric_limits<double>::min();
    double ymax = std::numeric_limits<double>::min();
};

#endif  // SILLY_UTILS_SILLY_DELAUNAY_H
