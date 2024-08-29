//
// Created by root on 2024/4/26.
//

#ifndef SILLY_UTILS_SILLY_OBJ_POOL_H
#define SILLY_UTILS_SILLY_OBJ_POOL_H
#include <su_marco.h>
template <typename T>
class silly_obj_pool
{
  public:
    // 获取一个对象
    std::unique_ptr<T> acquire()
    {
        if (m_pool.empty())
        {
            return std::make_unique<T>();
        }
        else
        {
            std::unique_ptr<T> obj = std::move(m_pool.front());
            m_pool.pop();
            return obj;
        }
    }

    // 释放一个对象，将其返回到池中
    void release(std::unique_ptr<T> obj)
    {
        m_pool.push(std::move(obj));
    }

    // 返回池中可用对象的数量
    size_t size() const
    {
        return m_pool.size();
    }

  private:
    size_t m_pnum{2};
    std::queue<std::unique_ptr<T>> m_pool;
};

#endif  // SILLY_UTILS_SILLY_OBJ_POOL_H
