#include "ThreadPool.h"

#include <iostream>

using namespace std;

void thread_func1()
{
    cout << "void thread_func1()" << endl;
}

int thread_func2(int i)
{
    this_thread::sleep_for(chrono::milliseconds(200));
    cout << "int thread_func2(int i), i = " << i << endl;
    return (i + 2);
}

void thread_func3(ThreadPool& tp, int i)
{
    if(i < 10)
    {
        tp.execute(thread_func3, ref(tp), i + 1);
    }

    cout << "void thread_func3(ThreadPool& tp, int i), i = " 
        << i << endl;
}

int main()
{
    cout << "==========simple thread pool==========" << endl;
    {
        ThreadPool tp(3);
        tp.clean();

        function< int(int) > f(thread_func2);
        tp.execute(thread_func1);
        tp.execute(f, 0);

        future< int > fut1 = tp.execute(thread_func2, 1);
        future< int > fut2 = tp.execute(bind(thread_func2, 4));
        future< void > fut3 = tp.execute(thread_func1);
        tp.execute([]() { cout << "lambda" << endl; });
        cout << "future1: " << fut1.get() << endl;
        cout << "future2: " << fut2.get() << endl;
        fut3.get();
        cout << "future3" << endl;
    }
 
    cout << "==========function loop==========" << endl;
    {
        ThreadPool tp(1);
        tp.execute(thread_func3, ref(tp), 0);
        this_thread::sleep_for(chrono::milliseconds(100));
        tp.terminate();
    }

    return 0;
}
