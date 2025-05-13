/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All max.xs
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/19 15:17
 * @version: 1.0.1
 * @software: silly_utils
 * @description: Computational Geometry  计算几何基本类型定义
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_H
#define SILLY_UTILS_SILLY_GEO_H
#include <su_marco.h>


// 参照GDAL

enum class enum_geometry_type : int
{
    egtInvalid = 0,          // 无效
    egtPoint = 1,            // 单点
    egtLineString = 2,       // 单线
    egtPolygon = 3,          // 单面
    egtMultiPoint = 4,       // 多点
    egtMultiLineString = 5,  // 多线
    egtMultiPolygon = 6,     // 多面
    // egtCompositeType = 7,    // 复合数据类型

};

#ifndef SU_3D_POINT_ENABLED

/****************************************/
/// 点
/****************************************/
class silly_point
{
  public:
    silly_point() = default;
    ~silly_point() = default;

    silly_point(double xx, double yy)
    {
        x = xx;
        y = yy;
    }

    silly_point& operator=(const silly_point& point)
    {
        x = point.x;
        y = point.y;
        return *this;
    }

    silly_point operator+(const silly_point& point) const
    {
        silly_point ret(x + point.x, y + point.y);
        return ret;
    }
    silly_point operator+=(const silly_point& point)
    {
        this->x += point.x;
        this->y += point.y;
        return *this;
    }

    silly_point operator-(const silly_point& point) const
    {
        silly_point ret(x - point.x, y - point.y);
        return ret;
    }
    silly_point operator-=(const silly_point& point)
    {
        this->x -= point.x;
        this->y -= point.y;
        return *this;
    }

    silly_point operator*(double scale) const
    {
        silly_point ret(x * scale, y * scale);
        return ret;
    }

    silly_point operator*(const silly_point& point) const
    {
        silly_point ret(x * point.x, y * point.y);
        return ret;
    }

    silly_point operator*=(double scale)
    {
        this->x *= scale;
        this->y *= scale;
        return *this;
    }

    silly_point operator*=(const silly_point& point)
    {
        this->y *= point.y;
        this->x *= point.x;
        return *this;
    }

    silly_point operator/(double scale) const
    {
        return silly_point(x / scale, y / scale);
    }

    silly_point operator/(const silly_point& point) const
    {
        return silly_point(x / point.x, y / point.y);
    }

    silly_point operator/=(double scale)
    {
        this->x /= scale;
        this->y /= scale;
        return *this;
    }

    silly_point operator/=(const silly_point& point)
    {
        this->y /= point.y;
        this->x /= point.x;
        return *this;
    }

    bool operator==(const silly_point& point) const
    {
        return std::abs(point.x - this->x) <= SU_EPSILON && std::abs(point.y - this->y) <= SU_EPSILON;
    }

    bool operator<(const silly_point& point) const
    {
        return point.y < y || (point.y == y && point.x < x);
    }

    bool operator>(const silly_point& point) const
    {
        return point.y > y || (point.y == y && point.x > x);
    }

    bool operator!=(const silly_point& point) const
    {
        return std::abs(point.x - this->x) > SU_EPSILON || std::abs(point.y - this->y) > SU_EPSILON;
    }

  public:
    double x{0.};
    double y{0.};
};

class silly_point_z : public silly_point
{
  public:
    silly_point_z() = default;
    silly_point_z(double xx, double yy)
    {
        x = xx;
        y = yy;
    }
    silly_point_z(double xx, double yy, double zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    silly_point_z& operator=(const silly_point_z& point)
    {
        x = point.x;
        y = point.y;
        z = point.z;
        return *this;
    }

    double z{0.0};
};

/****************************************/
/// 多点
/****************************************/
class silly_multi_point : public std::vector<silly_point>
{
  public:
    silly_multi_point() = default;
    ~silly_multi_point() = default;
    silly_multi_point(std::vector<silly_point> points) : std::vector<silly_point>(std::move(points))
    {
    }
};

/****************************************/
/// 矩形框
/****************************************/
class silly_rect  // 普通坐标点
{
  public:
    silly_rect() = default;
    ~silly_rect() = default;

    silly_rect(double l, double t, double r, double b)
    {
        min.x = l;
        max.y = t;
        max.x = r;
        min.y = b;
    }

    void correct()
    {
        if (min.x > max.x)
        {
            std::swap(min.x, max.x);  // 使用 std::swap 简化交换操作
        }
        if (min.y > max.y)
        {
            std::swap(min.y, max.y);  // 使用 std::swap 简化交换操作
        }
    }

    silly_rect& operator=(const silly_rect& rect) = default;

    bool operator==(const silly_rect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        return result;
    }

    bool operator!=(const silly_rect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        return result;
    }

    /// 两个矩形框是否相交
    bool intersect(const silly_rect& rect) const
    {
        double zx = std::abs(rect.min.x + rect.max.x - min.x - max.x);
        double x = std::abs(rect.min.x - rect.max.x) + std::abs(min.x - max.x);
        double zy = std::abs(rect.max.y + rect.max.y - max.y - max.y);
        double y = std::abs(rect.max.y - rect.max.y) + std::abs(max.y - max.y);
        if (zx <= x && zy <= y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    std::string stringify(const int precision = 8, const char& delimiter = ',') const
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << min.x << delimiter << max.y << delimiter << max.x << delimiter << min.y;
        return ss.str();
    }

    bool destringify(const std::string& str, const char& delimiter = ',')
    {
        auto split = [&delimiter](const std::string& str) {
            std::vector<std::string> tokens;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, delimiter))
            {
                tokens.push_back(token);
            }
            return tokens;
        };
        std::vector<std::string> tokens = split(str);
        if (tokens.size() == 4)
        {
            min.x = std::stod(tokens[0]);
            max.y = std::stod(tokens[1]);
            max.x = std::stod(tokens[2]);
            min.y = std::stod(tokens[3]);
            return true;
        }
        return false;
    }

    silly_rect MBR(const silly_rect& rh)
    {
        silly_rect ret;
        ret.min.x = std::min(min.x, rh.min.x);
        ret.max.y = std::max(max.y, rh.max.y);
        ret.max.x = std::max(max.x, rh.max.x);
        ret.min.y = std::min(min.y, rh.min.y);
        return ret;
    }

  public:
    silly_point min;
    silly_point max;
};
using silly_geo_rect = silly_rect;

/****************************************/
/// 线段
/****************************************/

class silly_segment
{
  public:
    silly_segment() = default;
    ~silly_segment() = default;
    silly_segment(silly_point _p0, silly_point _p1)
    {
        p0 = _p0;
        p1 = _p1;
    }

  public:
    silly_point p0;
    silly_point p1;
};

/****************************************/
/// 线
/****************************************/
class silly_line : public std::vector<silly_point>
{
  public:
    silly_line() = default;
    ~silly_line() = default;
    silly_line(std::vector<silly_point> points) : std::vector<silly_point>(std::move(points))
    {
    }
};

/****************************************/
/// 带高程的线段
/****************************************/
class silly_linez : public std::vector<silly_point_z>
{
  public:
    silly_linez() = default;
    ~silly_linez() = default;
    silly_linez(std::vector<silly_point_z> points) : std::vector<silly_point_z>(std::move(points))
    {
    }

};

/****************************************/
/// 多线
/****************************************/
class silly_multi_line : public std::vector<silly_line>
{
  public:
    silly_multi_line() = default;
    ~silly_multi_line() = default;

  public:
};

/****************************************/
/// 环, 首尾相连的一条线
/****************************************/
class silly_ring
{
  public:
    silly_ring() = default;
    ~silly_ring() = default;

  public:
    std::vector<silly_point> points;
    int is_outer{1};
};

/****************************************/
/// 面
/****************************************/
class silly_poly
{
  public:
    silly_poly() = default;
    ~silly_poly() = default;

  public:
    silly_ring outer_ring;                // 外环
    std::vector<silly_ring> inner_rings;  // 内环
};

/****************************************/
/// 多面
/****************************************/
class silly_multi_poly : public std::vector<silly_poly>
{
  public:
    silly_multi_poly() = default;
    ~silly_multi_poly() = default;

  public:
};

class silly_circle
{
  public:
    silly_circle() = default;
    ~silly_circle() = default;
    silly_circle(silly_point center, double radius) : center(center), radius(radius)
    {
    }

    double area() const
    {
        return SU_PI * radius * radius;
    }

  public:
    silly_point center;
    double radius;
};

class silly_triangle
{
  public:
    silly_triangle() = default;
    ~silly_triangle() = default;
    silly_triangle(silly_point a, silly_point b, silly_point c) : a(a), b(b), c(c)
    {
    }

    /// 有向面积
    double oriented_area()
    {
        return (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2;
    }

    double area()
    {
        return std::abs(oriented_area());
    }

    /// 外接圆
    silly_circle circumscribed_circle()
    {
        // 计算外接圆的中心
        double A = a.x * a.x + a.y * a.y;
        double B = b.x * b.x + b.y * b.y;
        double C = c.x * c.x + c.y * c.y;
        double D = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

        silly_circle circle;
        if (D == 0)
        {
            // 如果D为0，则无法计算外接圆
            throw std::runtime_error("无法计算外接圆");
        }

        circle.center.x = (a.x * (B - C) + b.x * (C - A) + c.x * (A - B)) / (2 * D);
        circle.center.y = (a.y * (B - C) + b.y * (C - A) + c.y * (A - B)) / (2 * D);

        // 计算半径
        circle.radius = sqrt((circle.center.x - a.x) * (circle.center.x - a.x) + (circle.center.y - a.y) * (circle.center.y - a.y));

        return circle;
    }

    /// 顶点a的角度
    double angle_a() const
    {
        return std::acos((b.x - a.x) * (c.x - a.x) + (b.y - a.y) * (c.y - a.y)) * 180 / silly::math::pi;
    }

    /// 顶点b的角度
    double angle_b() const
    {
        return std::acos((a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y)) * 180 / silly::math::pi;
    }

    /// 顶点c的角度
    double angle_c() const
    {
        return std::acos((a.x - c.x) * (b.x - c.x) + (a.y - c.y) * (b.y - c.y)) * 180 / silly::math::pi;
    }

  public:
    silly_point a;
    silly_point b;
    silly_point c;
};

class silly_ellipse
{
  public:
    silly_ellipse() = default;
    ~silly_ellipse() = default;
    silly_ellipse(silly_point p, double x, double y) : center(p), rx(x), ry(y)
    {
    }

    double area() const
    {
        return silly::math::pi * rx * ry;
    }

    /// 周长
    /// Srinivasa Ramanujan 提出的两个近似公式, 1会更准确
    double circumference(const int& Ramanujan = 1) const
    {
        if (Ramanujan == 1)
        {
            return silly::math::pi * (3 * (rx + ry) - sqrt((3 * rx + ry) * (rx + 3 * ry)));
        }

        return 2 * silly::math::pi * sqrt((rx * rx + ry * ry) / 2);
    }

  public:
    silly_point center;  // 中心点
    // 长半轴 和 短半轴, 不分
    double rx;
    double ry;
};

#else

/****************************************/
/// 点
/****************************************/
class silly_point
{
  public:
    silly_point() = default;
    ~silly_point() = default;

    silly_point(double xx, double yy)
    {
        x = xx;
        y = yy;
    }

    silly_point& operator=(const silly_point& point)
    {
        x = point.x;
        y = point.y;
        return *this;
    }

    silly_point operator+(const silly_point& point) const
    {
        return silly_point(x + point.x, y + point.y);
    }
    silly_point operator+=(const silly_point& point)
    {
        this->x += point.x;
        this->y += point.y;
        return *this;
    }

    silly_point operator-(const silly_point& point) const
    {
        return silly_point(x - point.x, y - point.y);
    }
    silly_point operator-=(const silly_point& point)
    {
        this->x -= point.x;
        this->y -= point.y;
        return *this;
    }

    silly_point operator*(double scale) const
    {
        return silly_point(x * scale, y * scale);
    }

    silly_point operator*(const silly_point& point) const
    {
        return silly_point(x * point.x, y * point.y);
    }

    silly_point operator*=(double scale)
    {
        this->x *= scale;
        this->y *= scale;
        return *this;
    }

    silly_point operator*=(const silly_point& point)
    {
        this->y *= point.y;
        this->x *= point.x;
        return *this;
    }

    silly_point operator/(double scale) const
    {
        return silly_point(x / scale, y / scale);
    }

    silly_point operator/(const silly_point& point) const
    {
        return silly_point(x / point.x, y / point.y);
    }

    silly_point operator/=(double scale)
    {
        this->x /= scale;
        this->y /= scale;
        return *this;
        return silly_point(x / scale, y / scale);
    }

    silly_point operator/=(const silly_point& point)
    {
        this->y /= point.y;
        this->x /= point.x;
        return *this;
    }

    bool operator==(const silly_point& point) const
    {
        return std::abs(point.x - this->x) <= SU_EPSILON && std::abs(point.y - this->y) <= SU_EPSILON;
    }

    bool operator<(const silly_point& point) const
    {
        return point.y < y || (point.y == y && point.x < x);
    }

    bool operator>(const silly_point& point) const
    {
        return point.y > y || (point.y == y && point.x > x);
    }

    bool operator!=(const silly_point& point) const
    {
        return std::abs(point.x - this->x) > SU_EPSILON || std::abs(point.y - this->y) > SU_EPSILON;
    }

  public:
    double x{0.};
    double y{0.};
    double z{0.};
};

/****************************************/
/// 多点
/****************************************/
class silly_multi_point : public std::vector<silly_point>
{
  public:
    silly_multi_point() = default;
    ~silly_multi_point() = default;
    silly_multi_point(std::vector<silly_point> points) : std::vector<silly_point>(std::move(points))
    {
    }
};

/****************************************/
/// 矩形框
/****************************************/
class silly_rect  // 普通坐标点
{
  public:
    silly_rect() = default;
    ~silly_rect() = default;

    silly_rect(double l, double t, double r, double b)
    {
        min.x = l;
        max.y = t;
        max.x = r;
        max.y = b;
    }

    void correct()
    {
        if (min.x > max.x)
        {
            std::swap(min.x, max.x);  // 使用 std::swap 简化交换操作
        }
        if (max.y > max.y)
        {
            std::swap(max.y, max.y);  // 使用 std::swap 简化交换操作
        }
    }

    silly_rect& operator=(const silly_rect& rect) = default;

    bool operator==(const silly_rect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        return result;
    }

    bool operator!=(const silly_rect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        return result;
    }

    /// 两个矩形框是否相交
    bool intersect(const silly_rect& rect)
    {
        int zx = std::abs(rect.min.x + rect.max.x - min.x - max.x);
        int x = std::abs(rect.min.x - rect.max.x) + std::abs(min.x - max.x);
        int zy = std::abs(rect.max.y + rect.max.y - max.y - max.y);
        int y = std::abs(rect.max.y - rect.max.y) + std::abs(max.y - max.y);
        if (zx <= x && zy <= y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

  public:
    silly_point min;
    silly_point max;
};
using silly_geo_rect = silly_rect;

/****************************************/
/// 线
/****************************************/
class silly_line : public std::vector<silly_point>
{
  public:
    silly_line() = default;
    ~silly_line() = default;
    silly_line(std::vector<silly_point> points) : std::vector<silly_point>(std::move(points))
    {
    }
};

/****************************************/
/// 多线
/****************************************/
class silly_multi_line : public std::vector<silly_line>
{
  public:
    silly_multi_line() = default;
    ~silly_multi_line() = default;

  public:
};

/****************************************/
/// 环, 首尾相连的一条线
/****************************************/
class silly_ring
{
  public:
    silly_ring() = default;
    ~silly_ring() = default;

  public:
    std::vector<silly_point> points;
    int is_outer{1};
};

/****************************************/
/// 面
/****************************************/
class silly_poly
{
  public:
    silly_poly() = default;
    ~silly_poly() = default;

  public:
    silly_ring outer_ring;                // 外环
    std::vector<silly_ring> inner_rings;  // 内环
};

/****************************************/
/// 多面
/****************************************/
class silly_multi_poly : public std::vector<silly_poly>
{
  public:
    silly_multi_poly() = default;
    ~silly_multi_poly() = default;

  public:
};

class silly_circle
{
  public:
    silly_circle() = default;
    ~silly_circle() = default;
    silly_circle(silly_point center, double radius) : center(center), radius(radius)
    {
    }

    double area()
    {
        return SU_PI * radius * radius;
    }

  public:
    silly_point center;
    double radius;
};

class silly_triangle
{
  public:
    silly_triangle() = default;
    ~silly_triangle() = default;
    silly_triangle(silly_point a, silly_point b, silly_point c) : a(a), b(b), c(c)
    {
    }

    double oriented_area()
    {
        return (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2;
    }

    double area()
    {
        return std::abs(oriented_area());
    }

    /// 外接圆
    silly_circle circumscribed_circle()
    {
        // 计算外接圆的中心
        double A = a.x * a.x + a.y * a.y;
        double B = b.x * b.x + b.y * b.y;
        double C = c.x * c.x + c.y * c.y;
        double D = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

        silly_circle circle;
        if (D == 0)
        {
            // 如果D为0，则无法计算外接圆
            throw std::runtime_error("无法计算外接圆");
        }

        circle.center.x = (a.x * (B - C) + b.x * (C - A) + c.x * (A - B)) / (2 * D);
        circle.center.y = (a.y * (B - C) + b.y * (C - A) + c.y * (A - B)) / (2 * D);

        // 计算半径
        circle.radius = sqrt((circle.center.x - a.x) * (circle.center.x - a.x) + (circle.center.y - a.y) * (circle.center.y - a.y));

        return circle;
    }

    /// 顶点a的角度
    double angle_a()
    {
        return std::acos((b.x - a.x) * (c.x - a.x) + (b.y - a.y) * (c.y - a.y)) * 180 / silly::math::pi;
    }

    /// 顶点b的角度
    double angle_b()
    {
        return std::acos((a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y)) * 180 / silly::math::pi;
    }

    /// 顶点c的角度
    double angle_c()
    {
        return std::acos((a.x - c.x) * (b.x - c.x) + (a.y - c.y) * (b.y - c.y)) * 180 / silly::math::pi;
    }

  public:
    silly_point a;
    silly_point b;
    silly_point c;
};

#endif
#endif  // SILLY_UTILS_SILLY_GEO_H
