#include "TaskGroup.h"

#include <iostream>

using namespace std;

int main()
{
    cout << "==========group all==========" << endl;
    {
        TaskGroup< int > tg1;
        tg1.Add(
            []() {
                cout << "f0_0" << endl;
                this_thread::sleep_for(chrono::seconds(2));
                cout << "f0_1" << endl;
                return 0;
            },
            []() { 
                cout << "f1_0" << endl;
                this_thread::sleep_for(chrono::seconds(1));
                cout << "f1_1" << endl;
                return 1;
            }
        );

        auto r1 = tg1.All().Run().get();
        for(auto& r : r1)
        {
            cout << "result: " << r << endl;
        }
    }
    
    cout << "==========group any==========" << endl;
    {
        TaskGroup< float > tg1;
        promise< float > val;
        tg1.Add(val.get_future());
        tg1.Add(
            []() {
                cout << "f0_0" << endl;
                this_thread::sleep_for(chrono::seconds(3)); 
                cout << "f0_1" << endl;
                return 1.0f; 
            },
            []() {
                cout << "f1_0" << endl;
                this_thread::sleep_for(chrono::seconds(2));
                cout << "f1_1" << endl;
                return 2.0f;
            }
        );

        //val.set_value(3.0f);

        auto r1 = tg1.Any().Run().get();
        cout << "index: " << r1.first 
             << " value: " << r1.second << endl;
    }

    return 0;
}