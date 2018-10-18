#ifndef CXX_TOOLS_DELAY_TASK_QUEUE_H
#define CXX_TOOLS_DELAY_TASK_QUEUE_H

#include "ConcurrentQueue.h"

#include <thread>
#include <functional>
#include <vector>

// similar to ThreadPool
class DelayTaskQueue
{
public:
    explicit DelayTaskQueue(int num_threads)
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
                        auto now = std::chrono::high_resolution_clock::now();
                        auto headTime = getHeadTime();
                        if(headTime <= now)
                        {
                            TimedFunction tf = tq.getFront();
                            lk.unlock();
                            tf.func();
                            lk.lock();
                        }
                        else
                        {
                            tq.wait_until(lk, headTime);
                        }
                    }
                    else
                    {
                        tq.wait(lk);
                    }
                }
            }));
        }
    }

    DelayTaskQueue(const DelayTaskQueue&) = delete;
    DelayTaskQueue& operator=(const DelayTaskQueue&) = delete;

    ~DelayTaskQueue()
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

        TimedFunction tf;
        tf.func = [taskPtr]() { (*taskPtr)(); };
        tf.time = std::chrono::high_resolution_clock::now();

        tq.emplace(std::move(tf));
        tq.notify_one();

        return taskPtr->get_future();
    }

    template< typename Rep, typename Period, typename F, typename... Args >
    auto execute_for(const std::chrono::duration< Rep, Period >& duration, 
                     F&& f, Args&&... args)
        -> std::future< decltype(f(args...)) >
    {
        using rtype = decltype(f(args...));

        auto taskPtr = std::make_shared< std::packaged_task< rtype() > >(
            std::bind(std::forward< F >(f), std  ::forward< Args >(args)...));

        TimedFunction tf;
        tf.func = [taskPtr]() { (*taskPtr)(); };
        tf.time = std::chrono::high_resolution_clock::now() + duration;

        tq.emplace(std::move(tf));
        tq.notify_one();

        return taskPtr->get_future();
    }

    template< typename Clock, typename Duration, typename F, typename... Args >
    auto execute_until(const std::chrono::time_point< Clock, Duration >& time, 
                       F&& f, Args&&... args)
        -> std::future< decltype(f(args...)) >
    {
        using rtype = decltype(f(args...));

        auto taskPtr = std::make_shared< std::packaged_task< rtype() > >(
            std::bind(std::forward< F >(f), std::forward< Args >(args)...));

        TimedFunction tf;
        tf.func = [taskPtr]() { (*taskPtr)(); };
        tf.time = time;

        tq.emplace(std::move(tf));
        tq.notify_one();

        return taskPtr->get_future();
    }

    void clean()
    {
        tq.clean();
    }

private:
    struct TimedFunction {
        std::function< void() > func;
        std::chrono::high_resolution_clock::time_point time;

        inline bool operator<(const TimedFunction& tf) const
        {
            // earlier function first
            return (time > tf.time);
        }
    };

    inline std::chrono::high_resolution_clock::time_point getHeadTime()
    {
        const TimedFunction& tf = tq.Front();
        return tf.time;
    }

    bool stop;
    ConcurrentQueue< TimedFunction, qPRIORITY > tq;
    std::vector< std::thread > threads;
};

#endif // CXX_TOOLS_DELAY_TASK_QUEUE_H