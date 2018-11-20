#ifndef CXX_TOOLS_READ_WRITE_LOCK_H
#define CXX_TOOLS_READ_WRITE_LOCK_H

// refer to: https://blog.csdn.net/mymodian9612/article/details/52794980

#include <mutex>
#include <condition_variable>

class ReadWriteLock
{
public:
    void LockRead()
    {
        std::unique_lock< std::mutex > lk(mtx);
        while(write_count > 0)
        {
            cond_read.wait(lk);
        }

        read_count += 1;
        status = kRead;
    }

    void UnlockRead()
    {
        std::unique_lock< std::mutex > lk(mtx);
        status = kIdle;
        if(--read_count == 0 && write_count > 0)
        {    
            cond_write.notify_one();
        }
    }

    void LockWrite()
    {
        std::unique_lock< std::mutex > lk(mtx);
        write_count += 1;
        
        while(status != kIdle || read_count > 0)
        {
            cond_write.wait(lk);
        }

        status = kWrite;
    }

    void UnlockWrite()
    {
        std::unique_lock< std::mutex > lk(mtx);
        status = kIdle;
        if(--write_count == 0)
        {
            cond_read.notify_all();
        }
        else
        {
            cond_write.notify_one();
        }
    }

private:
    std::mutex mtx;
    std::condition_variable cond_read;
    std::condition_variable cond_write;

    enum Status {
        kIdle, kRead, kWrite
    };

    Status status { kIdle };
    size_t read_count { 0 }; // locked
    size_t write_count { 0 }; //locked and waited
};

template< typename RWLock >
class UniqueLock
{
public:
    explicit UniqueLock(RWLock& _lk)
        : lk(_lk)
    {
        lk.LockWrite();
    }

    ~UniqueLock()
    {
        lk.UnlockWrite();
    }

    UniqueLock(const UniqueLock&) = delete;
    UniqueLock& operator=(const UniqueLock&) = delete;

private:
    RWLock& lk;
};

template< typename RWLock >
class SharedLock
{
public:
    explicit SharedLock(RWLock& _lk)
        : lk(_lk)
    {
        lk.LockRead();
    }

    ~SharedLock()
    {
        lk.UnlockRead();
    }

    SharedLock(const SharedLock&) = delete;
    SharedLock& operator=(const SharedLock&) = delete;

private:
    RWLock& lk;
};

#endif // CXX_TOOLS_READ_WRITE_LOCK_H