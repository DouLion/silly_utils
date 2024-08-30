/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_safe_stack.h
 * @description: silly_safe_stack 类声明
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SAFE_STACK_H
#define SILLY_UTILS_SILLY_SAFE_STACK_H

#include <su_marco.h>
template <typename V>
class silly_safe_stack
{
    typedef std::stack<V> StackType;

  public:
    /// Default Constructor.
    silly_safe_stack(void){};
    /// Destructor.
    virtual ~silly_safe_stack(void){};

    /// Push stack.
    bool Push(const V& value)
    {
        std::scoped_lock lock(m_recMutex);
        m_stack.push(value);

        return true;
    }

    /// Pop stack.
    bool Pop(V& value)
    {
        std::scoped_lock lock(m_recMutex);
        if (!m_stack.empty())
        {
            value = m_stack.top();
            m_stack.pop();

            return true;
        }

        return true;
    }

    /// Get stack size.
    int GetSize()
    {
        int size = m_stack.size();
        return size;
    }

    /// Whether is empty.
    bool IsEmpty()
    {
        bool bEmpty = m_stack.empty();
        return bEmpty;
    }

    /// Clear stack.
    bool Clear()
    {
        std::scoped_lock lock(m_recMutex);
        while (!m_stack.empty())
        {
            V& value = m_stack.top();
            m_stack.pop();
        }
        return true;
    }

  protected:
    /// Boost recursive mutex.
    std::mutex m_recMutex;
    /// Stack structure.
    StackType m_stack;
};

#endif  // SILLY_UTILS_SILLY_SAFE_STACK_H
