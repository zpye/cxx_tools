#ifndef CXX_TOOLS_TASK_QUEUE_H
#define CXX_TOOLS_TASK_QUEUE_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <functional>
#include <vector>
#include <queue>

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

enum TaskQueueType {
    qFIFO,
    qPRIORITY
};

template< typename T, TaskQueueType = qFIFO >
class TaskQueue {};

template< typename T >
class TaskQueue< T, qFIFO > : public BlockUnit
{
public:
    TaskQueue() : stop(false) {}

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

#endif // CXX_TOOLS_TASK_QUEUE_H