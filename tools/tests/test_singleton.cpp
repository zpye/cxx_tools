#include "Singleton.h"

#include <iostream>

using namespace std;

class A : public Singleton_v2< A >
{
private: 
    friend class Singleton_v2< A >;
    A(int i = 0)
    {
        cout << "constructor A(int i), i = " << i << endl;
    }
};

class B
{
public:
    B()
    {
        cout << "constructor B()" << endl;
    }
};

class C : public Singleton< C >
{
private:
    friend class Singleton< C >;
    C()
    {
        cout << "constructor C()" << endl;
    }
};

int main()
{
    cout << "==========Singleton==========" << endl;
    {
        A* inst1 = A::GetInstance(1);
        A* inst2 = A::GetInstance();
        A* inst3 = A::GetInstance(2);
        cout << "inst1: " << inst1 << endl;
        cout << "inst2: " << inst2 << endl;
        cout << "inst3: " << inst3 << endl;

        B& inst4 = Singleton< B >::GetInstance();
        B& inst5 = Singleton< B >::GetInstance();
        cout << "&inst4: " << &inst4 << endl;
        cout << "&inst5: " << &inst5 << endl;

        C& inst6 = C::GetInstance();
        C& inst7 = C::GetInstance();
        cout << "&inst6: " << &inst6 << endl;
        cout << "&inst7: " << &inst7 << endl;
    }

    return 0;
}