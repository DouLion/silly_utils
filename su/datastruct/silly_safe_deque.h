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
template <typename V>
class silly_safe_deque
{
    typedef std::deque<V> DequeType;

  public:
    /// Default Constructor.
    silly_safe_deque(void){};
    /// Destructor.
    virtual ~silly_safe_deque(void){};

    /// Push item to deque tail.
    bool PushItem(const V& value)
    {
        std::scoped_lock lock(m_recMutex);
        m_deque.push_back(value);

        return true;
    }

    /// Push item to deque head.
    bool PushItemToFront(const V& value)
    {
        std::scoped_lock lock(m_recMutex);
        m_deque.push_front(value);

        return true;
    }

    /// Pop item from deque.
    bool PopItem(V& value)
    {
        std::scoped_lock lock(m_recMutex);
        if (!m_deque.empty())
        {
            value = m_deque.front();
            m_deque.pop_front();

            return true;
        }

        return true;
    }

    /// Get deque size.
    int GetSize()
    {
        int size = m_deque.size();
        return size;
    }

    /// Whether the deque is empty.
    bool IsEmpty()
    {
        bool bEmpty = m_deque.empty();
        return bEmpty;
    }

    /// Clear deque.
    bool Clear()
    {
        std::scoped_lock lock(m_recMutex);
        m_deque.clear();

        return true;
    }

  protected:
    /// Boost recursive mutex.
    std::mutex m_recMutex;
    /// Deque structure.
    DequeType m_deque;
};

#endif  // SILLY_UTILS_SILLY_SAFE_DEQUE_H
