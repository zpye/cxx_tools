#ifndef CXX_TOOLS_SINGLETON_H
#define CXX_TOOLS_SINGLETON_H

#include <iostream>
#include <memory>
#include <mutex>
#include <utility>

// use local static
template <typename T>
class Singleton {
 public:
  static T& GetInstance() {
    static T inst;
    return inst;
  }

  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

 protected:
  Singleton() = default;
  ~Singleton() = default;
};

// use call_once
template <typename T>
class Singleton_v2 {
 public:
  template <typename... Args>
  static T& GetInstance(Args&&... args) {
    std::call_once(
        Singleton_v2<T>::oc,
        [&](Args&&... args) { inst.reset(new T(std::forward<Args>(args)...)); },
        std::forward<Args>(args)...);
    return (*inst);
  }

  Singleton_v2(const Singleton_v2&) = delete;
  Singleton_v2& operator=(const Singleton_v2&) = delete;

 protected:
  Singleton_v2() = default;
  ~Singleton_v2() = default;

 private:
  static std::unique_ptr<T> inst;
  static std::once_flag oc;
};

template <typename T>
std::unique_ptr<T> Singleton_v2<T>::inst(nullptr);

template <typename T>
std::once_flag Singleton_v2<T>::oc;

// use macro
#define TO_SINGLETON(className)                    \
 public:                                           \
  className(const className&) = delete;            \
  className& operator=(const className&) = delete; \
  static className& GetInstance() {                \
    static className inst;                         \
    return inst;                                   \
  }                                                \
                                                   \
 protected:                                        \
  className() = default;                           \
  ~className() = default

#endif  // CXX_TOOLS_SINGLETON_H