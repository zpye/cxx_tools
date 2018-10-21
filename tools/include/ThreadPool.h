#ifndef CXX_TOOLS_THREAD_POOL_H
#define CXX_TOOLS_THREAD_POOL_H

#include "ConcurrentQueue.h"

#include <thread>
#include <functional>
#include <vector>

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
                while(true)
                {
                    if(!tq.empty())
                    {
                        std::function< void() > task = tq.getFront();
                        lk.unlock();
                        task();
                        lk.lock();
                    }
                    else if(stop)
                    {
                        break;
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
        terminate(true);
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

    void terminate(bool cleanQueue = false)
    {
        tq.terminate();
        if(cleanQueue)
        {
            tq.clean();
        }

        {
            std::unique_lock< std::mutex > lk(tq.getLock());
            stop = true;
        }

        tq.notify_all();

        for(auto& t : threads)
        {
            if(t.joinable())
            {
                t.join();
            }
        }
    }

    void clean()
    {
        tq.clean();
    }

private:
    bool stop;
    ConcurrentQueue< std::function< void() > > tq;
    std::vector< std::thread > threads;
};

#endif // CXX_TOOLS_THREAD_POOL_H