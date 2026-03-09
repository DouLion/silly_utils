/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-02-04
 * @file: silly_geo_template
 * @description: silly_geo_template 头文件
 * @version: v1.0.1 2026-02-04 dou li yang
 */
#ifndef SILLY_GEO_TEMPLATE_H
#define SILLY_GEO_TEMPLATE_H
#include <vector>

template <typename T>
class suVecTemplate
{
    using ItemIt = typename std::vector<T>::iterator;
    using ConstItemIt = typename std::vector<T>::const_iterator;

  public:
    // 默认构造
    suVecTemplate() = default;

    // 从 std::vector<T> 拷贝构造
    explicit suVecTemplate(const std::vector<T>& items) : m_items(items)  // 直接初始化，高效且清晰
    {
    }

    // 支持移动构造（更高效）
    explicit suVecTemplate(std::vector<T>&& items) : m_items(std::move(items))
    {
    }
    // 代理vector的常用接口
    void push_back(const T& p)
    {
        m_items.push_back(p);
    }

    void push_back(T&& p)
    {
        m_items.push_back(std::move(p));
    }

    void pop_back()
    {
        m_items.pop_back();
    }

    size_t size() const
    {
        return m_items.size();
    }

    bool empty() const
    {
        return m_items.empty();
    }

    // 元素访问（引用传递，避免拷贝)
    T& operator[](size_t pos)
    {
        return m_items[pos];
    }

    const T& operator[](size_t pos) const
    {
        return m_items[pos];
    }

    T& front()
    {
        return m_items.front();
    }

    const T& front() const
    {
        return m_items.front();
    }

    T& back()
    {
        return m_items.back();
    }

    const T& back() const
    {
        return m_items.back();
    }

    // 迭代器支持（兼容范围for循环)
    ItemIt begin()
    {
        return m_items.begin();
    }

    ItemIt end()
    {
        return m_items.end();
    }

    ConstItemIt begin() const
    {
        return m_items.begin();
    }

    ConstItemIt end() const
    {
        return m_items.end();
    }

    ConstItemIt cbegin() const
    {
        return m_items.cbegin();
    }

    ConstItemIt cend() const
    {
        return m_items.cend();
    }

    void clear()
    {
        m_items.clear();
    }

    void reserve(size_t new_cap)
    {
        m_items.reserve(new_cap);
    }

    size_t capacity() const
    {
        return m_items.capacity();
    }

    void resize(size_t count)
    {
        m_items.resize(count);
    }

    void resize(size_t count, const T& value)
    {
        m_items.resize(count, value);
    }

    // 安全访问
    T& at(size_t pos)
    {
        return m_items.at(pos);
    }

    const T& at(size_t pos) const
    {
        return m_items.at(pos);
    }

    // 底层指针
    T* data() noexcept
    {
        return m_items.data();
    }

    const T* data() const noexcept
    {
        return m_items.data();
    }

    // 高效构造
    template <typename... Args>
    void emplace_back(Args&&... args)
    {
        m_items.emplace_back(std::forward<Args>(args)...);
    }

    void swap(suVecTemplate& other) noexcept
    {
        m_items.swap(other.m_items);
    }
    std::vector<T> to_vector() const
    {
        return m_items;
    }

  protected:
    std::vector<T> m_items;
};
#endif
