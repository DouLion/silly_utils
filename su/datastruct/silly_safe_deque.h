/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_safe_deque.h
 * @description: silly_safe_deque 类声明
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SAFE_DEQUE_H
#define SILLY_UTILS_SILLY_SAFE_DEQUE_H
#include <su_marco.h>
template <typename T>
class silly_safe_deque
{
    typedef std::deque<T> type;

  public:
    /// Default Constructor.
    silly_safe_deque(void) {};
    /// Destructor.
    virtual ~silly_safe_deque(void) {};

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
    /// Boost recursive mutex.
    std::mutex m_mutex;
    /// Deque structure.
    type m_deque;
};

#endif  // SILLY_UTILS_SILLY_SAFE_DEQUE_H
