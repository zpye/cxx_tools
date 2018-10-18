#ifndef CXX_TOOLS_CONCURRENT_QUEUE_H
#define CXX_TOOLS_CONCURRENT_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <chrono>
#include <type_traits>

class BlockUnit
{
public:
    inline std::unique_lock< std::mutex > getLock()
    {
        return std::move(std::unique_lock< std::mutex >(mtx));
    }

    inline void wait(std::unique_lock< std::mutex >& lk)
    {
        cond.wait(lk);
    }

    template< typename Rep, typename Period >
    inline std::cv_status wait_for(std::unique_lock< std::mutex >& lk,
           const std::chrono::duration< Rep, Period >& rel_time)
    {
        return cond.wait_for(lk, rel_time);
    }

    template< typename Clock, typename Duration >
    inline std::cv_status wait_until(std::unique_lock< std::mutex >& lk, 
           const std::chrono::time_point< Clock, Duration >& timeout_time)
    {
        return cond.wait_until(lk, timeout_time);
    }

    inline void notify_one()
    {
        cond.notify_one();
    }

    inline void notify_all()
    {
        cond.notify_all();
    }

protected:
    std::mutex mtx;
    std::condition_variable cond;
};

enum ConcurrentQueueType {
    qFIFO,
    qPRIORITY
};

template< typename T, ConcurrentQueueType = qFIFO >
class ConcurrentQueue {};

template< typename T >
class ConcurrentQueue< T, qFIFO > : public BlockUnit
{
public:
    ConcurrentQueue() : stop(false) {}

    // getFront and empty are not thread safe
    inline T getFront() // queue must not empty
    {
        T t = std::move(tasks.front());
        tasks.pop();
        return std::move(t);
    }

    inline bool empty()
    {
        return tasks.empty();
    }

    inline bool getFrontSafe(T& t)
    {
        std::lock_guard< std::mutex > lk(mtx);
        if(!stop && !empty())
        {
            t = std::move(tasks.front());
            tasks.pop();
            return true;
        }

        return false;
    }

    inline void emplace(T& task)
    {
        std::lock_guard< std::mutex > lk(mtx);
        if(!stop)
        {
            tasks.emplace(task);
        }
    }

    inline void emplace(T&& task)
    {
        std::lock_guard< std::mutex > lk(mtx);
        if(!stop)
        {
            tasks.emplace(std::move(task));
        }
    }

    inline void clean()
    {
        std::lock_guard< std::mutex > lk(mtx);
        tasks = std::queue< T >();
    }

    inline void terminate()
    {
        std::lock_guard< std::mutex > lk(mtx);
        stop = true;
    }

private:
    bool stop;
    std::queue< T > tasks;
};

template< typename T >
class ConcurrentQueue< T, qPRIORITY > : public BlockUnit
{
public:
    ConcurrentQueue() : stop(false) {}

    // getFront, Front and empty are not thread safe
    inline T getFront() // queue must not empty
    {
        T t = std::move(tasks.top());
        tasks.pop();
        return std::move(t);
    }

    inline const T& Front()
    {
        return tasks.top();
    }

    inline bool empty()
    {
        return tasks.empty();
    }

    inline bool getFrontSafe(T& t)
    {
        std::lock_guard< std::mutex > lk(mtx);
        if(!stop && !empty())
        {
            t = tasks.top();
            tasks.pop();
            return true;
        }

        return false;
    }

    inline void emplace(T& task)
    {
        std::lock_guard< std::mutex > lk(mtx);
        if(!stop)
        {
            tasks.emplace(task);
        }
    }

    inline void emplace(T&& task)
    {
        std::lock_guard< std::mutex > lk(mtx);
        if(!stop)
        {
            tasks.emplace(std::move(task));
        }
    }

    inline void clean()
    {
        std::lock_guard< std::mutex > lk(mtx);
        tasks = std::priority_queue< T >();
    }

    inline void terminate()
    {
        std::lock_guard< std::mutex > lk(mtx);
        stop = true;
    }

private:
    bool stop;
    std::priority_queue< T > tasks;
};

#endif // CXX_TOOLS_CONCURRENT_QUEUE_H