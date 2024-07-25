#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <su_marco.h>

// silly_task interface (function or callable object)
using silly_task = std::function<void()>;

class silly_thread_pool
{
  public:
    silly_thread_pool(unsigned int numThreads)
    {
        for (unsigned int i = 0; i < numThreads; ++i)
        {
            workers.emplace_back([this] {
                for (;;)
                {
                    silly_task task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    //template <typename Func, typename... Args>
    //auto enqueue(Func&& f, Args&&... args) -> std::future<decltype(f(args...))>
    //{
    //    using return_type = decltype(f(args...));

    //    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));

    //    std::future<return_type> res = task->get_future();
    //    {
    //        std::unique_lock<std::mutex> lock(queue_mutex);

    //        // don't allow enqueueing after stopping the pool
    //        if (stop)
    //            throw std::runtime_error("enqueue on stopped silly_thread_pool");

    //        tasks.emplace([task]() { (*task)(); });
    //    }
    //    condition.notify_one();
    //    return res;
    //}

    template <typename Func, typename... Args>
    auto enqueue(Func&& f, Args&&... args) -> std::future<void>
    {
        using return_type = void;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));

        std::future<void> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }


    ~silly_thread_pool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }

  private:
    std::vector<std::thread> workers;
    std::queue<silly_task> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
};
