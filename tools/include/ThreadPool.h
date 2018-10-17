#ifndef CXX_TOOLS_THREAD_POOL_H
#define CXX_TOOLS_THREAD_POOL_H

#include "TaskQueue.h"

class ThreadPool
{
public:
    explicit ThreadPool(size_t num_threads)
        : stop(false)
    {
        for(size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(std::thread(
            [&]() {
                std::unique_lock< std::mutex > lk(tq.getLock());
                while(!stop)
                {
                    if(!tq.empty())
                    {
                        std::function< void() > task = tq.getFront();
                        lk.unlock();
                        task();
                        lk.lock();
                    }
                    else
                    {
                        tq.wait(lk);
                    }
                }
            }));
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool()
    {
        {
            std::unique_lock< std::mutex > lk(tq.getLock());
            stop = true;
        }

        tq.terminate();
        tq.notify_all();

        for(auto& t : threads)
        {
            if(t.joinable())
            {
                t.join();
            }
        }
    }

    template< typename F, typename... Args >
    auto execute(F&& f, Args&&... args)
        -> std::future< decltype(f(args...)) >
    {
        using rtype = decltype(f(args...));
        
        auto taskPtr = std::make_shared< std::packaged_task< rtype() > >(
            std::bind(std::forward< F >(f), std::forward< Args >(args)...));

        tq.emplace([taskPtr]() { (*taskPtr)(); });
        tq.notify_one();

        return taskPtr->get_future();
    }

    void clean()
    {
        tq.clean();
    }

private:
    bool stop;
    TaskQueue< std::function< void() > > tq;
    std::vector< std::thread > threads;
};

#endif // CXX_TOOLS_THREAD_POOL_H