#include "Reflection.h"

#include <iostream>

using namespace std;

class ReflectionBase
{
public:
    virtual void run() = 0;
};

class Reflection1 : public ReflectionBase
{
public:
    void run() override
    {
        cout << "Reflection1 run" << endl;
    }
};

REGIST(Reflection1)

class Reflection2 : public ReflectionBase
{
public:
    void run() override
    {
        cout << "Reflection2 run" << endl;
    }
};

REGIST(Reflection2)

class Reflection3 : public ReflectionBase
{
public:
    void run() override
    {
        cout << "Reflection3 address: " << this << endl;
    }
};

REGIST_SINGLETON(Reflection3)

int main()
{
    cout << "==========simple reflection==========" << endl;
    {
        AbstractFactory& factory = AbstractFactory::GetInstance();
        ReflectionBase* r1 = (ReflectionBase *)factory.GetClassByName("Reflection1");
        ReflectionBase* r2 = (ReflectionBase *)factory.GetClassByName("Reflection2");
        ReflectionBase* r3 = (ReflectionBase *)factory.GetClassByName("Reflection3");
        ReflectionBase* r4 = (ReflectionBase *)factory.GetClassByName("Reflection3");
        r1->run();
        r2->run();
        r3->run();
        r4->run();
    }

    return 0; 
}