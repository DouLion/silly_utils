/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_safe_list.h
 * @description: 线程安全的链表
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_SAFE_LIST_H
#define SILLY_SAFE_LIST_H
#include <su_macro.h>
template <typename T>
class suSafeList
{
  public:
    /// Default constructor.
    suSafeList() = default;
    /// Destructor.
    virtual ~suSafeList() = default;

    /// Add item.
    void push_back(const T& value)
    {
        std::scoped_lock lock(m_mutex);
        m_list.push_back(value);
    }

    /// Remove item.
    bool remove(const T& value)
    {
        std::scoped_lock lock(m_mutex);
        m_list.remove(value);

        return true;
    }

    /// Get list size.
    size_t size()
    {
        return m_list.size();
    }

    /// Whether is empty.
    bool empty()
    {
        return m_list.empty();
    }

    /// Clear list.
    void clear()
    {
        std::scoped_lock lock(m_mutex);
        m_list.clear();
    }

  protected:
    std::mutex m_mutex;
     std::list<T> m_list;
};

#endif  // SILLY_SAFE_LIST_H
