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

    /**
     * 向量的点积
     * 值 =0 两向量垂直
     * > 0 夹角 θ<90∘ (锐角)
     * < 0 夹角 θ>90∘ (钝角)
     * @param rh
     * @return
     */
    double dot2D(const suPoint& rh) const
    {
        return x * rh.x + y * rh.y;
    }

    /**
     * 向量的叉积
     *  > 0 rh 在左侧（或逆时针方向)
     *  < 0 rh 在右侧 (或顺时针方向)
     *  = 0 共线
     * @param rh
     * @return
     */
    double cross2D(const suPoint& rh) const
    {
        return x * rh.y - y * rh.x;
    }

    /**
     * 距离的平方
     * @param rh
     * @return
     */
    double dist2(const suPoint& rh) const
    {
        const double dx = (x - rh.x);
        const double dy = (y - rh.y);
        const double dz = (z - rh.z);
        return dx * dx + dy * dy + dz * dz;
    }

    /**
     * 距离
     * @param rh
     * @return
     */
    double dist(const suPoint& rh) const
    {
        return std::sqrt(dist2(rh));
    }

    /**
     * 交换x 和 y 坐标
     */
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

    // 元素访问（引用传递，避免拷贝)
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

    // 迭代器支持（兼容范围for循环)
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

/**
 * 点在面内的基本算法
 * @param point
 * @param ring 闭合环
 * @return
 */
bool SU_POINT_IN_CLOSED_RING(const suPoint& point, const std::vector<suPoint>& ring);

/**
 *  有向面积
 *  在计算机图形学和计算几何中，通过有向面积的正负
 *  可以判断多边形顶点的环绕顺序是顺时针（Clockwise, CW)还是逆时针（Counterclockwise, CCW)
 *  其核心原理是基于向量叉积或鞋带公式（Shoelace Formula)的符号特性.
 *  若结果为正：顶点按逆时针（CCW)顺序排列
 *  若结果为负：顶点按顺时针（CW)顺序排列
 * @param ring 自动认为首尾点闭合
 * @return
 */
double SU_CLOSED_RING_ORIENTED_AREA(const std::vector<suPoint>& ring);

/**
 * 实际算术面积
 * @param ring
 * @return
 */
double SU_CLOSED_RING_NORMAL_AREA(const std::vector<suPoint>& ring);

#endif  // SILLY_POINT_H
