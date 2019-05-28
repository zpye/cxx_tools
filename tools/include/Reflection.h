#ifndef CXX_TOOLS_REFLECTION_H
#define CXX_TOOLS_REFLECTION_H

#include <functional>
#include <map>
#include <string>

#include "Singleton.h"

template <typename T>
void* MakeInst() {
  return static_cast<void*>(new T);
}

template <typename T>
void* MakeInstSingleton() {
  return static_cast<void*>(&Singleton<T>::GetInstance());
}

using FuncBase = std::function<void*()>;

class AbstractFactory {
 public:
  void* GetClassByName(std::string name) {
    auto iter = records.find(name);
    if (iter == records.end()) {
      return nullptr;
    } else {
      return (iter->second)();
    }
  }

  void regist(std::string name, const FuncBase& func) {
    records.emplace(name, func);
  }

 private:
  std::map<std::string, FuncBase> records;

  TO_SINGLETON(AbstractFactory);
};

class ClassRegister {
 public:
  ClassRegister(std::string name, const FuncBase& func) {
    AbstractFactory::GetInstance().regist(name, func);
  }
};

#define REGIST(className)                                    \
  static ClassRegister ClassRegister_##className(#className, \
                                                 MakeInst<className>);

#define REGIST_SINGLETON(className)               \
  static ClassRegister ClassRegister_##className( \
      #className, MakeInstSingleton<className>);

#endif  // CXX_TOOLS_REFLECTION_H