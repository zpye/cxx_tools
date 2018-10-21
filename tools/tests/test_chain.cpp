#include "Chain.h"

#include <iostream>

using namespace std;

int chain_func(int i)
{
    return i + 1;
}

int main()
{
    cout << "==========simple chain==========" << endl;
    {
        Chain< int(int) > chain([](int i) { cout << "f1(int)->int" << endl; return i + 1; });
        auto result = chain.Then([](int i) { cout << "f2(int)->int" << endl; return i + 1; })
            .Then([](int i) { cout << "f3(int)->int" << endl; return i + 1; }).Run(0);
        cout << "chain result: " << result.get() << endl;

        auto result2 = chain.Run(5);
        cout << "chain result2: " << result2.get() << endl;

        Chain< int(int) > chain2 = 
        Chain< int(int) >([](int i) { cout << "f4(int)->int" << endl; return i + 2; })
            .Then([](int i) {cout << "f5(int)->int" << endl; return i + 2; });
        cout << "chain2 result: " << chain2.Run(1).get() << endl;

        Chain< int(int) > chain3 = chain2
            .Then([](int i) {cout << "f6(int)->int" << endl; return i + 2; });
        cout << "chain3 result: " << chain3.Run(5).get() << endl;
     }

    cout << "==========void return==========" << endl;
    {
        Chain< void(void) > chain([]() { cout << "f1(void)->void" << endl; });
        auto result = chain.Then([]() { cout << "f2(void)->void" << endl; })
            .Then([]() { cout << "f3(void)->void" << endl; }).Run();
        result.get();

        Chain< int(void) > chain2([]() { cout << "f4(void)->int" << endl; return 0; });
        auto result2 = chain2.Then([](int i) { cout << "f5(int)->void " << i << endl; })
            .Then([]() { cout << "f6(void)->int" << endl; return 1; })
            .Then([](int i) { cout << "f7(int)->void " << i << endl; }).Run();
        result2.get();
    }

    cout << "==========stl function==========" << endl;
    {
        Chain< int(int) > chain(chain_func);
        auto chain2 = chain.Then(chain_func)
            .Then(chain_func)
            .Then(chain_func)
            .Then(chain_func);
        auto result = chain2.Run(2);
        cout << "result: " << result.get() << endl;
    }

    return 0;
}