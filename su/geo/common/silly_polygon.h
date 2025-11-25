/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_polygon.cpp
 * @description: silly_polygon 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_POLYGON_H
#define SILLY_POLYGON_H
#include <geo/common/silly_point.h>
#include <geo/common/silly_rect.h>
class suRing
{
  public:
    std::vector<suPoint> points;
    int is_outer{1};

    double area() const;

    suRect bound() const;


};

/****************************************/
/// 面
/****************************************/
class suPoly
{
  public:
    suRing outer;               // 外环
    std::vector<suRing> holes;  // 内环, 孔, 洞

    double area() const;

    suRect bound() const;

};

/****************************************/
/// 多面
/****************************************/
class suMultiPoly
{
  public:
    suMultiPoly() = default;
    // 类型别名（兼容STL迭代器）
    using iterator = typename std::vector<suPoly>::iterator;
    using const_iterator = typename std::vector<suPoly>::const_iterator;
    using value_type = suPoly;  // 兼容STL容器类型定义
    explicit suMultiPoly(std::vector<suPoly> polys) : m_polys(std::move(polys))
    {
    }

    double area() const;

    suRect bound() const;

    // 代理vector的常用接口
    void push_back(const suPoly& p)
    {
        m_polys.push_back(p);
    }
    void push_back(suPoly&& p)
    {
        m_polys.push_back(std::move(p));
    }
    void pop_back()
    {
        m_polys.pop_back();
    }
    size_t size() const
    {
        return m_polys.size();
    }
    bool empty() const
    {
        return m_polys.empty();
    }

    // 元素访问（引用传递，避免拷贝）
    suPoly& operator[](size_t pos)
    {
        return m_polys[pos];
    }
    const suPoly& operator[](size_t pos) const
    {
        return m_polys[pos];
    }
    suPoly& front()
    {
        return m_polys.front();
    }
    const suPoly& front() const
    {
        return m_polys.front();
    }
    suPoly& back()
    {
        return m_polys.back();
    }
    const suPoly& back() const
    {
        return m_polys.back();
    }

    // 迭代器支持（兼容范围for循环）
    iterator begin()
    {
        return m_polys.begin();
    }
    iterator end()
    {
        return m_polys.end();
    }
    const_iterator begin() const
    {
        return m_polys.begin();
    }
    const_iterator end() const
    {
        return m_polys.end();
    }
    const_iterator cbegin() const
    {
        return m_polys.cbegin();
    }
    const_iterator cend() const
    {
        return m_polys.cend();
    }

    // 其他可能需要的方法（如clear、reserve等）
    void clear()
    {
        m_polys.clear();
    }
    void reserve(size_t n)
    {
        m_polys.reserve(n);
    }
    void resize(size_t n)
    {
        m_polys.resize(n);
    }

  protected:
    std::vector<suPoly> m_polys;
};
#endif  // SILLY_POLYGON_H
