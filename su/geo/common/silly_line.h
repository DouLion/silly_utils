/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suLine.cpp
 * @description: suLine 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_LINE_H
#define SILLY_LINE_H
#include <geo/common/silly_point.h>

class suLine
{
  public:
    // 类型别名（兼容STL迭代器）
    using iterator = typename std::vector<suPoint>::iterator;
    using const_iterator = typename std::vector<suPoint>::const_iterator;
    using value_type = suPoint;  // 兼容STL容器类型定义
    // 构造函数
    suLine() = default;
    explicit suLine(std::vector<suPoint> points) : m_points(std::move(points))
    {
    }

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

    // 其他可能需要的方法（如clear、reserve等）
    void clear()
    {
        m_points.clear();
    }
    void reserve(size_t n)
    {
        m_points.reserve(n);
    }

    /**
     * 构建等间距的折线
     * @param dist
     * @return
     */
    suLine equidistant(const double& dist) const;

    /**
     * 总线长度
     * @return
     */
    double distance() const;

  protected:
    std::vector<suPoint> m_points;
};

/****************************************/
/// 多线
/****************************************/
class suMultiLine
{
  public:
    // 类型别名（兼容STL迭代器）
    using iterator = typename std::vector<suLine>::iterator;
    using const_iterator = typename std::vector<suLine>::const_iterator;
    using value_type = suLine;  // 兼容STL容器类型定义
    suMultiLine() = default;
    explicit suMultiLine(std::vector<suLine> lines) : m_lines(std::move(lines))
    {
    }

    // 代理vector的常用接口
    void push_back(const suLine& p)
    {
        m_lines.push_back(p);
    }
    void push_back(suLine&& p)
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
    suLine& operator[](size_t pos)
    {
        return m_lines[pos];
    }
    const suLine& operator[](size_t pos) const
    {
        return m_lines[pos];
    }
    suLine& front()
    {
        return m_lines.front();
    }
    const suLine& front() const
    {
        return m_lines.front();
    }
    suLine& back()
    {
        return m_lines.back();
    }
    const suLine& back() const
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
    std::vector<suLine> m_lines;
};
using silly_multi_line = suLine;
#endif  // SILLY_LINE_H
