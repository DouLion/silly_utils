/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_line.cpp
 * @description: silly_line 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_LINE_H
#define SILLY_LINE_H
#include <geo/common/silly_point.h>

class silly_line
{
  public:
    // 类型别名（兼容STL迭代器）
    using iterator = typename std::vector<silly_point>::iterator;
    using const_iterator = typename std::vector<silly_point>::const_iterator;
    using value_type = silly_point;  // 兼容STL容器类型定义
    // 构造函数
    silly_line() = default;
    explicit silly_line(std::vector<silly_point> points) : m_points(std::move(points))
    {
    }

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

    // 其他可能需要的方法（如clear、reserve等）
    void clear()
    {
        m_points.clear();
    }
    void reserve(size_t n)
    {
        m_points.reserve(n);
    }

  protected:
    std::vector<silly_point> m_points;
};

/****************************************/
/// 带高程的线段
/****************************************/
class silly_lineZ
{
  public:
    // 类型别名（兼容STL迭代器）
    using iterator = typename std::vector<silly_pointZ>::iterator;
    using const_iterator = typename std::vector<silly_pointZ>::const_iterator;
    using value_type = silly_pointZ;  // 兼容STL容器类型定义
    // 构造函数
    silly_lineZ() = default;
    explicit silly_lineZ(std::vector<silly_pointZ> points) : m_points(std::move(points))
    {
    }

    // 代理vector的常用接口
    void push_back(const silly_pointZ& p)
    {
        m_points.push_back(p);
    }
    void push_back(silly_pointZ&& p)
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
    silly_pointZ& operator[](size_t pos)
    {
        return m_points[pos];
    }
    const silly_pointZ& operator[](size_t pos) const
    {
        return m_points[pos];
    }
    silly_pointZ& front()
    {
        return m_points.front();
    }
    const silly_pointZ& front() const
    {
        return m_points.front();
    }
    silly_pointZ& back()
    {
        return m_points.back();
    }
    const silly_pointZ& back() const
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

    // 其他可能需要的方法（如clear、reserve等）
    void clear()
    {
        m_points.clear();
    }
    void reserve(size_t n)
    {
        m_points.reserve(n);
    }

  public:
    std::vector<silly_pointZ> m_points;
};

/****************************************/
/// 多线
/****************************************/
class silly_multi_line
{
  public:
    // 类型别名（兼容STL迭代器）
    using iterator = typename std::vector<silly_line>::iterator;
    using const_iterator = typename std::vector<silly_line>::const_iterator;
    using value_type = silly_line;  // 兼容STL容器类型定义
    silly_multi_line() = default;
    explicit silly_multi_line(std::vector<silly_line> lines) : m_lines(std::move(lines))
    {
    }

    // 代理vector的常用接口
    void push_back(const silly_line& p)
    {
        m_lines.push_back(p);
    }
    void push_back(silly_line&& p)
    {
        m_lines.push_back(std::move(p));
    }
    void pop_back()
    {
        m_lines.pop_back();
    }
    size_t size() const
    {
        return m_lines.size();
    }
    bool empty() const
    {
        return m_lines.empty();
    }

    // 元素访问（引用传递，避免拷贝）
    silly_line& operator[](size_t pos)
    {
        return m_lines[pos];
    }
    const silly_line& operator[](size_t pos) const
    {
        return m_lines[pos];
    }
    silly_line& front()
    {
        return m_lines.front();
    }
    const silly_line& front() const
    {
        return m_lines.front();
    }
    silly_line& back()
    {
        return m_lines.back();
    }
    const silly_line& back() const
    {
        return m_lines.back();
    }

    // 迭代器支持（兼容范围for循环）
    iterator begin()
    {
        return m_lines.begin();
    }
    iterator end()
    {
        return m_lines.end();
    }
    const_iterator begin() const
    {
        return m_lines.begin();
    }
    const_iterator end() const
    {
        return m_lines.end();
    }
    const_iterator cbegin() const
    {
        return m_lines.cbegin();
    }
    const_iterator cend() const
    {
        return m_lines.cend();
    }

    // 其他可能需要的方法（如clear、reserve等）
    void clear()
    {
        m_lines.clear();
    }
    void reserve(size_t n)
    {
        m_lines.reserve(n);
    }

  protected:
    std::vector<silly_line> m_lines;
};

#endif  // SILLY_LINE_H
