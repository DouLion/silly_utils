/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_other.cpp
 * @description: 点定义,最多支持 三维坐标点(x, y, z) 及此点对应的值v
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_POINT_H
#define SILLY_POINT_H

#include <su_marco.h>
// 支持外面覆盖这个值
#ifndef SU_GEO_EPSILON
#define SU_GEO_EPSILON (1e-8)
#endif
class suPoint
{
    /*不要扩散概念, 避免出现意外的错误*/
  public:
    double x = 0.0;         // 横坐标
    double y = 0.0;         // 纵坐标
    double z = 0.0;         // 高
    double v = 0.0;         // 值

  public:
    suPoint() = default;
    ~suPoint() = default;

    suPoint(const double& xx, const double& yy)
    {
        x = xx;
        y = yy;
    }

    suPoint(const double& xx, const double& yy, const double& zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    suPoint(const double& xx, const double& yy, const double& zz, const double& vv)
    {
        x = xx;
        y = yy;
        z = zz;
        v = vv;
    }

    suPoint operator+(const suPoint& rh) const
    {
        suPoint ret(x + rh.x, y + rh.y, z + rh.z);
        return ret;
    }

    suPoint& operator+=(const suPoint& rh)
    {
        x += rh.x;
        y += rh.y;
        z += rh.z;
        return *this;
    }

    suPoint operator-(const suPoint& rh) const
    {
        suPoint ret(x - rh.x, y - rh.y, z - rh.z);
        return ret;
    }
    suPoint& operator-=(const suPoint& rh)
    {
        x -= rh.x;
        y -= rh.y;
        z -= rh.z;
        return *this;
    }

    suPoint operator*(const double& scale) const
    {
        suPoint ret(x * scale, y * scale, z * scale);
        return ret;
    }

    suPoint& operator*=(const double& scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;
        return *this;
    }

    suPoint operator/(const double& scale) const
    {
        return suPoint(x / scale, y / scale, z / scale);
    }

    suPoint& operator/=(const double& scale)
    {
        x /= scale;
        y /= scale;
        z /= scale;
        return *this;
    }

    // 重写比较函数
    bool operator==(const suPoint& rh) const
    {
        return rh.x == this->x && rh.y == this->y && rh.z == this->z;
    }

    bool operator<=(const suPoint& rh) const
    {
        return std::tie(z, y, x) <= std::tie(rh.z, rh.y, rh.x);
    }

    bool operator<(const suPoint& rh) const
    {
        return std::tie(z, y, x) < std::tie(rh.z, rh.y, rh.x);
    }

    bool operator>=(const suPoint& rh) const
    {
        return std::tie(z, y, x) >= std::tie(rh.z, rh.y, rh.x);
    }

    bool operator>(const suPoint& rh) const
    {
        return std::tie(z, y, x) > std::tie(rh.z, rh.y, rh.x);
    }

    bool operator!=(const suPoint& rh) const
    {
        return !(*this == rh);
    }

    //
    double dot(const suPoint& rh) const
    {
        return x * rh.x + y * rh.y + z * rh.z;
    }

    double cross(const suPoint& rh) const
    {
        return x * rh.y - y * rh.x;  // 2D 叉积（标量）
    }

    double distance(const suPoint& rh) const
    {
        const double dx = (x - rh.x);
        const double dy = (y - rh.y);
        const double dz = (z - rh.z);
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
    void swap()
    {
        std::swap(x, y);
    }
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
