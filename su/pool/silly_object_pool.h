/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/4/26
 * @version: 1.0.1
 * @description: 对象池
 */
#ifndef SILLY_OBJECT_POOL_H
#define SILLY_OBJECT_POOL_H
#include <su_macro.h>
template <typename T>
class suObjPool
{
public:
    // 可选：自定义重置函数类型
    using ResetFunction = std::function<void(T&)>;

    // 构造函数：预分配 n 个对象
    explicit suObjPool(size_t initialSize = 2, ResetFunction resetFn = {})
        : m_resetFn(std::move(resetFn))
    {
        for (size_t i = 0; i < initialSize; ++i) {
            m_pool.emplace(std::make_unique<T>());
        }
    }

    // 获取一个对象（线程安全）
    std::unique_ptr<T> acquire()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (m_pool.empty()) {
            // 池空时创建新对象（可选：也可抛异常或阻塞）
            return std::make_unique<T>();
        }

        auto obj = std::move(m_pool.front());
        m_pool.pop();
        return obj;
    }

    // 释放对象回池（线程安全）
    void release(std::unique_ptr<T> obj)
    {
        if (!obj) return; // 防止空指针

        // 重置对象状态（关键！）
        if (m_resetFn) {
            m_resetFn(*obj);
        } else {
            // 默认：调用析构 + 重新构造（placement new）
            obj.reset(); // 先析构
            obj = std::make_unique<T>(); // 重新构造（简单但有效）
            // 注意：更高效的做法是提供 T 的 reset() 方法
        }

        std::lock_guard<std::mutex> lock(m_mtx);
        m_pool.push(std::move(obj));
    }

    // 返回当前池中可用对象数量（线程安全）
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_pool.size();
    }

private:
    mutable std::mutex m_mtx;
    ResetFunction m_resetFn;
    std::queue<std::unique_ptr<T>> m_pool;
};
#endif  // SILLY_OBJECT_POOL_H
