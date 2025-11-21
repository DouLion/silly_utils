/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_other.cpp
 * @description: suPoint 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_POINT_H
#define SILLY_POINT_H

#include <su_marco.h>
#define SU_GEO_EPSILON (1e-8)
class suPoint
{
  public:
    suPoint() = default;
    ~suPoint() = default;

    suPoint(const double& xx, const double& yy)
    {
        x = xx;
        y = yy;
    }

    suPoint& operator=(const suPoint& rh) = default;

    suPoint operator+(const suPoint& rh) const
    {
        suPoint ret(x + rh.x, y + rh.y);
        return ret;
    }

    double dot(const suPoint& rh) const
    {
        return x * rh.x + y * rh.y;
    }

    double cross(const suPoint& rh) const
    {
        return x * rh.y - y * rh.x;  // 2D 叉积（标量）
    }

    suPoint operator+=(const suPoint& rh)
    {
        this->x += rh.x;
        this->y += rh.y;
        return *this;
    }

    suPoint operator-(const suPoint& rh) const
    {
        suPoint ret(x - rh.x, y - rh.y);
        return ret;
    }
    suPoint operator-=(const suPoint& rh)
    {
        this->x -= rh.x;
        this->y -= rh.y;
        return *this;
    }

    suPoint operator*(double scale) const
    {
        suPoint ret(x * scale, y * scale);
        return ret;
    }

    suPoint operator*(const suPoint& rh) const
    {
        suPoint ret(x * rh.x, y * rh.y);
        return ret;
    }

    suPoint operator*=(double scale)
    {
        this->x *= scale;
        this->y *= scale;
        return *this;
    }

    suPoint operator*=(const suPoint& rh)
    {
        this->y *= rh.y;
        this->x *= rh.x;
        return *this;
    }

    suPoint operator/(double scale) const
    {
        return suPoint(x / scale, y / scale);
    }

    suPoint operator/(const suPoint& rh) const
    {
        return suPoint(x / rh.x, y / rh.y);
    }

    suPoint operator/=(double scale)
    {
        this->x /= scale;
        this->y /= scale;
        return *this;
    }

    suPoint operator/=(const suPoint& rh)
    {
        this->y /= rh.y;
        this->x /= rh.x;
        return *this;
    }

    bool operator==(const suPoint& rh) const
    {
        return std::abs(rh.x - this->x) <= SU_GEO_EPSILON && std::abs(rh.y - this->y) <= SU_GEO_EPSILON;
    }

    bool operator<(const suPoint& rh) const
    {
        return rh.y < y || (rh.y == y && rh.x < x);
    }

    bool operator>(const suPoint& rh) const
    {
        return rh.y > y || (rh.y == y && rh.x > x);
    }

    bool operator!=(const suPoint& rh) const
    {
        return std::abs(rh.x - this->x) > SU_GEO_EPSILON || std::abs(rh.y - this->y) > SU_GEO_EPSILON;
    }

    double distance(const suPoint& rh) const
    {
        return SU_DIST(x - rh.x, y - rh.y);
    }

    void swap()
    {
        std::swap(x, y);
    }

  public:
    double x{0.};
    double y{0.};
};
using silly_point = suPoint;
class suPointZ : public suPoint
{
  public:
    suPointZ() = default;
    suPointZ(const double& xx, const double& yy)
    {
        x = xx;
        y = yy;
    }
    suPointZ(const double& xx, const double& yy, const double& zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    suPointZ(const suPoint& rh, const double& zz)
    {
        x = rh.x;
        y = rh.y;
        z = zz;
    }

    suPointZ(const suPoint& rh)
    {
        x = rh.x;
        y = rh.y;
    }

    suPointZ(const suPointZ& rh) : suPoint(rh)
    {
        x = rh.x;
        y = rh.y;
        z = rh.z;
    }

    suPointZ& operator=(const suPointZ& rh)
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
class suMultiPoint
{
  public:
    suMultiPoint() = default;
    ~suMultiPoint() = default;
    /*suMultiPoint(const suMultiPoint& rh);
    suMultiPoint(const std::vector<suPoint>& rh);*/
    using iterator = typename std::vector<suPoint>::iterator;
    using const_iterator = typename std::vector<suPoint>::const_iterator;
    using value_type = suPoint;  // 兼容STL容器类型定义

    // 代理vector的常用接口
    void push_back(const suPoint& p)
    {
        m_points.push_back(p);
    }
    void push_back(suPoint&& p)
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
    suPoint& operator[](size_t pos)
    {
        return m_points[pos];
    }
    const suPoint& operator[](size_t pos) const
    {
        return m_points[pos];
    }
    suPoint& front()
    {
        return m_points.front();
    }
    const suPoint& front() const
    {
        return m_points.front();
    }
    suPoint& back()
    {
        return m_points.back();
    }
    const suPoint& back() const
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
    std::vector<suPoint> m_points;
};

#endif  // SILLY_POINT_H
