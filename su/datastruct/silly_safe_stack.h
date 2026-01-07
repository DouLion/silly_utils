/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_safe_stack.h
 * @description: 线程安全的栈类
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_SAFE_STACK_H
#define SILLY_SAFE_STACK_H

#include <su_macro.h>
template <typename T>
class suSafeStack
{
  public:
    /// Default Constructor.
    suSafeStack() = default;
    /// Destructor.
    virtual ~suSafeStack() = default;

    /// Push stack.
    void push(const T& value)
    {
        std::scoped_lock lock(m_mutex);
        m_stack.push(value);
    }

    /// Pop stack.
    void pop(T& value)
    {
        std::scoped_lock lock(m_mutex);
        if (!m_stack.empty())
        {
            value = m_stack.top();
            m_stack.pop();
        }
    }

    /// Get stack size.
    size_t size()
    {
        return m_stack.size();
    }

    /// Whether is empty.
    bool empty()
    {
        return m_stack.empty();
    }

    /// Clear stack.
    void clear()
    {
        std::scoped_lock lock(m_mutex);
        while (!m_stack.empty())
        {
            m_stack.pop();
        }
    }

  protected:
    std::mutex m_mutex;
    std::stack<T> m_stack;
};

#endif  // SILLY_SAFE_STACK_H
