/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_other.cpp
 * @description: silly_point 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_POINT_H
#define SILLY_POINT_H

#include <su_marco.h>
#define SU_GEO_EPSILON (1e-8)
class silly_point
{
  public:
    silly_point() = default;
    ~silly_point() = default;

    silly_point(const double& xx, const double& yy)
    {
        x = xx;
        y = yy;
    }

    silly_point& operator=(const silly_point& other) = default;

    silly_point operator+(const silly_point& other) const
    {
        silly_point ret(x + other.x, y + other.y);
        return ret;
    }

    double dot(const silly_point& other) const
    {
        return x * other.x + y * other.y;
    }

    double cross(const silly_point& other) const
    {
        return x * other.y - y * other.x;  // 2D 叉积（标量）
    }

    silly_point operator+=(const silly_point& other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    silly_point operator-(const silly_point& other) const
    {
        silly_point ret(x - other.x, y - other.y);
        return ret;
    }
    silly_point operator-=(const silly_point& other)
    {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    silly_point operator*(double scale) const
    {
        silly_point ret(x * scale, y * scale);
        return ret;
    }

    silly_point operator*(const silly_point& other) const
    {
        silly_point ret(x * other.x, y * other.y);
        return ret;
    }

    silly_point operator*=(double scale)
    {
        this->x *= scale;
        this->y *= scale;
        return *this;
    }

    silly_point operator*=(const silly_point& other)
    {
        this->y *= other.y;
        this->x *= other.x;
        return *this;
    }

    silly_point operator/(double scale) const
    {
        return silly_point(x / scale, y / scale);
    }

    silly_point operator/(const silly_point& other) const
    {
        return silly_point(x / other.x, y / other.y);
    }

    silly_point operator/=(double scale)
    {
        this->x /= scale;
        this->y /= scale;
        return *this;
    }

    silly_point operator/=(const silly_point& other)
    {
        this->y /= other.y;
        this->x /= other.x;
        return *this;
    }

    bool operator==(const silly_point& other) const
    {
        return std::abs(other.x - this->x) <= SU_GEO_EPSILON && std::abs(other.y - this->y) <= SU_GEO_EPSILON;
    }

    bool operator<(const silly_point& other) const
    {
        return other.y < y || (other.y == y && other.x < x);
    }

    bool operator>(const silly_point& other) const
    {
        return other.y > y || (other.y == y && other.x > x);
    }

    bool operator!=(const silly_point& other) const
    {
        return std::abs(other.x - this->x) > SU_GEO_EPSILON || std::abs(other.y - this->y) > SU_GEO_EPSILON;
    }

  public:
    double x{0.};
    double y{0.};
};

class silly_pointZ : public silly_point
{
  public:
    silly_pointZ() = default;
    silly_pointZ(const double& xx, const double& yy)
    {
        x = xx;
        y = yy;
    }
    silly_pointZ(const double& xx, const double& yy, const double& zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    silly_pointZ(const silly_point& rh)
    {
        x = rh.x;
        y = rh.y;
    }

    silly_pointZ(const silly_pointZ& rh) : silly_point(rh)
    {
        x = rh.x;
        y = rh.y;
        z = rh.z;
    }

    silly_pointZ& operator=(const silly_pointZ& rh)
    {
        x = rh.x;
        y = rh.y;
        z = rh.z;
        return *this;
    }

    double z{0.0};
};

/****************************************/
/// 多点
/****************************************/
class silly_multi_point
{
  public:
    silly_multi_point() = default;
    ~silly_multi_point() = default;
    /*silly_multi_point(const silly_multi_point& rh);
    silly_multi_point(const std::vector<silly_point>& rh);*/
    using iterator = typename std::vector<silly_point>::iterator;
    using const_iterator = typename std::vector<silly_point>::const_iterator;
    using value_type = silly_point;  // 兼容STL容器类型定义

    // 代理vector的常用接口
    void push_back(const silly_point& p)
    {
        m_points.push_back(p);
    }
    void push_back(silly_point&& p)
    {
        m_points.push_back(std::move(p));
    }
    void pop_back()
    {
        m_points.pop_back();
    }
    size_t size() const
    {
        return m_points.size();
    }
    bool empty() const
    {
        return m_points.empty();
    }

    // 元素访问（引用传递，避免拷贝）
    silly_point& operator[](size_t pos)
    {
        return m_points[pos];
    }
    const silly_point& operator[](size_t pos) const
    {
        return m_points[pos];
    }
    silly_point& front()
    {
        return m_points.front();
    }
    const silly_point& front() const
    {
        return m_points.front();
    }
    silly_point& back()
    {
        return m_points.back();
    }
    const silly_point& back() const
    {
        return m_points.back();
    }

    // 迭代器支持（兼容范围for循环）
    iterator begin()
    {
        return m_points.begin();
    }
    iterator end()
    {
        return m_points.end();
    }
    const_iterator begin() const
    {
        return m_points.begin();
    }
    const_iterator end() const
    {
        return m_points.end();
    }
    const_iterator cbegin() const
    {
        return m_points.cbegin();
    }
    const_iterator cend() const
    {
        return m_points.cend();
    }

  protected:
    std::vector<silly_point> m_points;
};

#endif  // SILLY_POINT_H
