#include "DelayTaskQueuel.h"

#include <iostream>

using namespace std;

void DelayFunc1()
{
    cout << "DelayFunc1()->void" << endl;
}

int DelayFunc2(int i)
{
    cout << "DelayFunc2(int)->int" << endl;
    return i + 1;
}

int DelayFunc3(int i, int j)
{
    cout << "DelayFunc3(int, int)->int" << endl;
    return i + j;
}

int main()
{
    cout << "==========used as ThreadPool==========" << endl;
    {
        DelayTaskQueue dtq(2);
        future< void > f1 = dtq.execute(DelayFunc1);
        future< int > f2 = dtq.execute(DelayFunc2, 1);
        future< int > f3 = dtq.execute(DelayFunc3, 2, 3);
        future< void > f4 = dtq.execute(DelayFunc1);
        future< int > f5 = dtq.execute(DelayFunc2, 4);
        future< int > f6 = dtq.execute(DelayFunc3, 5, 6);

        f1.get();
        cout << "f1" << endl;
        cout << "f2: " << f2.get() << endl;
        cout << "f3: " << f3.get() << endl;
        f4.get();
        cout << "f4" << endl;
        cout << "f5: " << f5.get() << endl;
        cout << "f6: " << f6.get() << endl;
    }

    cout << "==========delay task==========" << endl;
    {
        DelayTaskQueue dtq(2);
        future< void > f1 = 
            dtq.execute_for(chrono::seconds(1), DelayFunc1);
        future< int > f2 = 
            dtq.execute_until(chrono::high_resolution_clock::now() + chrono::seconds(2),
                              DelayFunc2, 1);
        future< int > f3 = 
            dtq.execute_for(chrono::seconds(3), DelayFunc3, 2, 3);
        future< void > f4 = 
            dtq.execute_until(chrono::high_resolution_clock::now(), 
                              DelayFunc1);
        future< int > f5 = 
            dtq.execute_for(chrono::seconds(2), DelayFunc2, 4);
        future< int > f6 = 
            dtq.execute_until(chrono::high_resolution_clock::now() + chrono::seconds(1), 
                              DelayFunc3, 5, 6);

        f1.get();
        cout << "f1" << endl;
        cout << "f2: " << f2.get() << endl;
        cout << "f3: " << f3.get() << endl;
        f4.get();
        cout << "f4" << endl;
        cout << "f5: " << f5.get() << endl;
        cout << "f6: " << f6.get() << endl;
    }

    return 0;
}