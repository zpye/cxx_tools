/**
 * refer to "Protothreads"(http://dunkels.com/adam/pt/index.html) 
 * and its c++ implement(https://github.com/benhoyt/protothreads-cpp)
*/

#ifndef CXX_TOOLS_COROUTINE_H
#define CXX_TOOLS_COROUTINE_H

#include <cstdint>

class Protothreads
{
public:
    Protothreads() : ptLine(0) {}
    
    virtual bool Run() = 0;
    
    bool isRunning()
    {
        return (ptLine != lineInvalid);
    }

    void Stop()
    {
        ptLine = lineInvalid;
    }

    void Restart()
    {
        ptLine = 0;
    }

protected:
    uint32_t ptLine;
    static const uint32_t lineInvalid = static_cast< uint32_t >(-1);
};

#define PT_BEGIN() bool ptYielded = true; switch (ptLine) { case 0:

#define PT_END() default: ; } Stop(); return false;

#define PT_WAIT_UNTIL(condition) \
    do { ptLine = __LINE__; if(!(condition)) return true; case __LINE__: ; } while(0)

#define PT_WAIT_WHILE(condition) PT_WAIT_UNTIL(!(condition))

#define PT_WAIT_THREAD(child) PT_WAIT_WHILE((child).Run())

#define PT_SPAWN(child) \
    do { (child).Restart(); PT_WAIT_THREAD(child); } while(0)

#define PT_RESTART() do { Restart(); return true; } while(0)

#define PT_EXIT() do { Stop(); return false; } while(0)

#define PT_YIELD() \
    do { ptYielded = false; ptLine = __LINE__; if(!ptYielded) return true; case __LINE__: ; } while(0)

#define PT_YIELD_UNTIL(condition) \
    do { ptYielded = false; ptLine = __LINE__; if(!ptYielded || !(condition)) return true; case __LINE__: ; } while(0)

#endif // CXX_TOOLS_COROUTINE_H