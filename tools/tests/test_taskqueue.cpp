#include "TaskQueue.h"

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

int main()
{
    cout << "==========simple value==========" << endl;
    {
        TaskQueue< int > tq1;
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

        TaskQueue< std::string > tq2;
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
        TaskQueue< Task > tq3;
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

    return 0;
}