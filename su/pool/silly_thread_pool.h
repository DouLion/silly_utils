/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/08/07 10:16
 * @version: 1.0.1
 * @description: 线程池 copy from https://github.com/progschj/ThreadPool
 */
#ifndef SILLY_THREAD_POOL_H
#define SILLY_THREAD_POOL_H
#pragma once
#include <su_macro.h>

class suThreadPool
{
  public:
    explicit suThreadPool(const size_t& threads = std::thread::hardware_concurrency());
    ~suThreadPool();

    /**
     * 添加计算任务
     * @param f  函数
     * @param args  参数
     * @return
     */
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

  private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> m_workers;
    // 任务队列
    std::queue<std::function<void()> > m_tasks;

    // 同步
    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
    bool m_stop;
};


// add new work item to the pool
template <class F, class... Args>
auto suThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()> >(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (m_stop)
        {
            throw std::runtime_error("停止的线程池不应该添加计算队列");
        }

        m_tasks.emplace([task]() { (*task)(); });
    }
    m_condition.notify_one();
    return res;
}

using silly_thread_pool = suThreadPool;
#endif
/*
* A simple C++11 Thread Pool implementation
* License FreeBSD
Copyright (c) 2012 Jakob Progsch, Václav Zeman

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/