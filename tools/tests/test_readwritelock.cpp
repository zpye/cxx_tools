#include "ReadWriteLock.h"

#include <iostream>
#include <vector>
#include <thread>

using namespace std;

static int num = 0;
static ReadWriteLock rw_lock;
void RWtest(int n)
{
    for(int i = 0; i < n; ++i)
    {
        {
            SharedLock< ReadWriteLock > lk(rw_lock);
            cout << "num: " << num << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(5));
    }

    UniqueLock< ReadWriteLock > lk(rw_lock);
    num += 1;
    cout << "num changed: " << num << endl;
}

void Wtest(int n)
{
    for(int i = 0; i < n; ++i)
    {
        UniqueLock< ReadWriteLock > lk(rw_lock);
        num += 1;
        cout << "num changed: " << num << endl;
    }
}

int main()
{
    cout << "read-write test" << endl;
    {
        vector< thread > ths;
        for(int i = 1; i <= 10; ++i)
        {
            ths.push_back(thread(RWtest, i * 10));
        }

        for(auto& th : ths)
        {
            th.join();
        }
    }

    cout << "write test" << endl;
    {
        vector< thread > ths;
        for(int i = 1; i <= 10; ++i)
        {
            ths.push_back(thread(Wtest, i));
        }

        for(auto& th : ths)
        {
            th.join();
        }
    }

    return 0;
}