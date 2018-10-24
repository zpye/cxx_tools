#ifndef CXX_TOOLS_REFLECTION_H
#define CXX_TOOLS_REFLECTION_H

#include <string>
#include <map>
#include <type_traits>

#include "Singleton.h"

class ReturnFuncBase
{
public:
    virtual void* operator()(void) = 0;
};

template< typename T >
class ReturnFunc : public ReturnFuncBase
{
public:
    void* operator()(void) override
    {
        return static_cast< void* >(new T);
    }
};

template< typename T >
class ReturnFuncSingleton : public ReturnFuncBase
{
public:
    void* operator()(void) override
    {
        return static_cast< void* >(&Singleton< T >::GetInstance());
    }
};

class AbstractFactory
{
public:
    void* GetClassByName(std::string name)
    {
        auto iter = records.find(name);
        if(iter == records.end())
        {
            return nullptr;
        }
        else
        {
            return (*(iter->second))();
        }
    }

    void regist(std::string name, ReturnFuncBase* func)
    {
        records.emplace(name, func);
    }

private:
    std::map< std::string, ReturnFuncBase* > records;

    TO_SINGLETON(AbstractFactory)
};

class ClassRegister
{
public:
    ClassRegister(std::string name, ReturnFuncBase* func)
    {
        AbstractFactory::GetInstance().regist(name, func);
    }
};

#define REGIST(className) \
    static ReturnFunc<className> ReturnFunc_##className; \
    static ClassRegister ClassRegister_##className(#className, \
            &ReturnFunc_##className);

#define REGIST_SINGLETON(className) \
    static ReturnFuncSingleton<className> ReturnFunc_##className; \
    static ClassRegister ClassRegister_##className(#className, \
            &ReturnFunc_##className);

#endif // CXX_TOOLS_REFLECTION_H