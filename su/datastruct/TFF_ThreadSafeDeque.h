//////////////////////////////////////////////////////////////////////////
//
//	@file		TFF_ThreadSafeDeque.h
//	@brief	TFF_ThreadSafeDeque header file
//	@author		gaoyang
//  @version    1.0
//	@date		2017-03-01
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <mutex>
#include <deque>

/** @class ThreadSafeDeque
        @brief The class of thread safe deque.
        @remark
*/
template <typename V>
class ThreadSafeDeque
{
    typedef std::deque<V> DequeType;

  public:
    /// Default Constructor.
    ThreadSafeDeque(void){};
    /// Destructor.
    virtual ~ThreadSafeDeque(void){};

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
