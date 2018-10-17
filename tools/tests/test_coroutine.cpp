/**
 * refer to "Protothreads"(http://dunkels.com/adam/pt/index.html) 
 * and its c++ implement(https://github.com/benhoyt/protothreads-cpp)
*/

#include "Coroutine.h"

#include <iostream>
#include <memory>

using namespace std;

using PTS = struct ProtothreadStatics {
    static bool flagA;
    static bool flagB;
};

bool PTS::flagA = false;
bool PTS::flagB = false;
//using PTS = struct ProtothreadStatics;

class ProtoA : public Protothreads
{
public:
    bool Run() override;

private:
    size_t i;
};

bool ProtoA::Run()
{
    PT_BEGIN();

    for(i = 0; i < 5; ++i)
    {
        PT_WAIT_UNTIL(PTS::flagB);
        cout << "ProtoA i: " << i << endl;
        
        PTS::flagB = false;
        PTS::flagA = true;
    }

    PT_END();
}

class ProtoB : public Protothreads
{
public:
    bool Run() override;

private:
    size_t i;
};

bool ProtoB::Run()
{
    PT_BEGIN();

    for(i = 0; i < 10; ++i)
    {
        PT_WAIT_UNTIL(PTS::flagA);
        cout << "ProtoB i: " << i << endl;
        
        PTS::flagA = false;
        PTS::flagB = true;
    }

    PT_END();
}

class ProtoC : public Protothreads
{
public:
    bool Run() override;

private:
    size_t i;
};

bool ProtoC::Run()
{
    PT_BEGIN();

    for(i = 0; i < 6; ++i)
    {
        cout << "ProtoC i: " << i << endl;
        PT_YIELD();
    }

    cout << "ProtoC end" << endl;

    PT_END();
}

class Driver
{
public:
    Driver()
    {
        ptA = make_shared< ProtoA >();
        ptB = make_shared< ProtoB >();
    }

    bool Run();
    
private:
    shared_ptr< ProtoA > ptA;
    shared_ptr< ProtoB > ptB;
};

bool Driver::Run()
{
    PTS::flagA = false;
    PTS::flagB = true;

    cout << "Driver start" << endl;
    do
    {
        ptA->Run();
        ptB->Run();
    } while(ptA->isRunning() || ptB->isRunning());
    cout << "Driver end" << endl;

    return true;
}

int main()
{
    cout << "==========coroutine==========" << endl;
    {
        Driver driver;
        driver.Run();
    }

    cout << "==========yield==========" << endl;
    {
        ProtoC ptC;
        while(ptC.Run());
    }

    return 0;
}