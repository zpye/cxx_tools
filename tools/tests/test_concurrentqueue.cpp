#include "ConcurrentQueue.h"

#include <iostream>
#include <string>

using namespace std;

class Task
{
public:
    Task(int _i = 0) : i(_i) {}

    void Run()
    {
        cout << "Task i = " << i << endl;
    }

    int i;
};

struct Level {
    Level(int _index) : index(_index) {}
    int index;

    void Run()
    {
        cout << "Level index = " << index << endl;
    }

    inline bool operator<(const Level& level) const
    {
        // top is the largest
        return (index < level.index);
    }
};

int main()
{
    cout << "==========simple value==========" << endl;
    {
        ConcurrentQueue< int > tq1;
        int i = 100;
        tq1.emplace(1);
        tq1.emplace(2);
        tq1.emplace(5);
        tq1.emplace(i);
        tq1.emplace(-3);

        while(!tq1.empty())
        {
            cout << "tq1: " << tq1.getFront() << endl;
        }

        ConcurrentQueue< std::string, qFIFO > tq2;
        std::string s("string");
        tq2.emplace("123");
        tq2.emplace("abc");
        tq2.emplace(s);

        while(!tq2.empty())
        {
            cout << "tq2: " << tq2.getFront() << endl;
        }
    }

    cout << "==========class==========" << endl;
    {
        ConcurrentQueue< Task > tq3;
        Task task(10);
        tq3.emplace(Task());
        tq3.emplace(task);
        tq3.emplace(Task(-5));

        while(true)
        {
            Task t;
            if(tq3.getFrontSafe(t))
            {
                t.Run();
            }
            else
            {
                break;
            }
        }
    }

    cout << "==========priority==========" << endl;
    {
        ConcurrentQueue< Level, qPRIORITY > tq4;
        tq4.emplace(Level(0));
        tq4.emplace(Level(-1));
        tq4.emplace(Level(2));
        tq4.emplace(Level(-2));
        tq4.emplace(Level(1));

        while(!tq4.empty())
        {
            Level& level = tq4.getFront();
            level.Run();
        }
    }

    return 0;
}