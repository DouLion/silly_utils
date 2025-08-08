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
class silly_ring
{
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
    silly_ring outer;               // 外环
    std::vector<silly_ring> holes;  // 内环, 孔, 洞
};

/****************************************/
/// 多面
/****************************************/
class silly_multi_poly
{
public:
    silly_multi_poly() = default;
    // 类型别名（兼容STL迭代器）
    using iterator = typename std::vector<silly_poly>::iterator;
    using const_iterator = typename std::vector<silly_poly>::const_iterator;
    using value_type = silly_poly; // 兼容STL容器类型定义
    explicit silly_multi_poly(std::vector<silly_poly> lines)
        : m_polys(std::move(lines)) {}

    // 代理vector的常用接口
    void push_back(const silly_poly& p) { m_polys.push_back(p); }
    void push_back(silly_poly&& p) { m_polys.push_back(std::move(p)); }
    void pop_back() { m_polys.pop_back(); }
    size_t size() const { return m_polys.size(); }
    bool empty() const { return m_polys.empty(); }

    // 元素访问（引用传递，避免拷贝）
    silly_poly& operator[](size_t pos) { return m_polys[pos]; }
    const silly_poly& operator[](size_t pos) const { return m_polys[pos]; }
    silly_poly& front() { return m_polys.front(); }
    const silly_poly& front() const { return m_polys.front(); }
    silly_poly& back() { return m_polys.back(); }
    const silly_poly& back() const { return m_polys.back(); }

    // 迭代器支持（兼容范围for循环）
    iterator begin() { return m_polys.begin(); }
    iterator end() { return m_polys.end(); }
    const_iterator begin() const { return m_polys.begin(); }
    const_iterator end() const { return m_polys.end(); }
    const_iterator cbegin() const { return m_polys.cbegin(); }
    const_iterator cend() const { return m_polys.cend(); }

    // 其他可能需要的方法（如clear、reserve等）
    void clear() { m_polys.clear(); }
    void reserve(size_t n) { m_polys.reserve(n); }
    void resize(size_t n) { m_polys.resize(n); }

protected:
    std::vector<silly_poly> m_polys;
};


#endif //SILLY_POLYGON_H
