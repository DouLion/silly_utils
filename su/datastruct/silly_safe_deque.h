/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_safe_deque.h
 * @description: suSafeDeque 类声明
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_SAFE_DEQUE_H
#define SILLY_SAFE_DEQUE_H
#include <su_macro.h>
template <typename T>
class suSafeDeque
{
  public:
    /// Default Constructor.
    suSafeDeque() = default;
    /// Destructor.
    virtual ~suSafeDeque() = default;

    /// Push item to deque tail.
    void push_back(const T& value)
    {
        std::scoped_lock lock(m_mutex);
        m_deque.push_back(value);
    }

    /// Push item to deque head.
    void push_front(const T& value)
    {
        std::scoped_lock lock(m_mutex);
        m_deque.push_front(value);
    }

    /// Pop item from deque.
    void pop_front(T& value)
    {
        std::scoped_lock lock(m_mutex);
        if (!m_deque.empty())
        {
            m_deque.pop_front();
        }
    }

    /// Get deque size.
    size_t size()
    {
        return m_deque.size();
    }

    /// Whether the deque is empty.
    bool empty()
    {
        return m_deque.empty();
    }

    /// Clear deque.
    void clear()
    {
        std::scoped_lock lock(m_mutex);
        m_deque.clear();
    }

  protected:
    std::mutex m_mutex;
    std::deque<T> m_deque;
};

#endif  // SILLY_SAFE_DEQUE_H
