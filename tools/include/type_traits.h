#ifndef CXX_TOOLS_TYPE_TRAITS_H
#define CXX_TOOLS_TYPE_TRAITS_H

#include <cstdint>
#include <type_traits>

template <typename T>
struct func_traits;

template <typename R, typename... Args>
struct func_traits<R(Args...)> {
  using return_type = R;
};

// from C++ head file "memory" in visual studio
template <class U, class T>
struct is_convertible_ext : std::is_convertible<U*, T*>::type {};

template <class U, class T>
struct is_convertible_ext<U, T[]>
    : std::is_convertible<U (*)[], T (*)[]>::type {};

template <class U, class T, size_t N>
struct is_convertible_ext<U, T[N]>
    : std::is_convertible<U (*)[N], T (*)[N]>::type {};

template <class U, class T>
struct is_pointer_compatible_ext : std::is_convertible<U*, T*>::type {};

template <class U, size_t N>
struct is_pointer_compatible_ext<U[N], U[]> : std::true_type {};

template <class U, size_t N>
struct is_pointer_compatible_ext<U[N], const U[]> : std::true_type {};

template <class U, size_t N>
struct is_pointer_compatible_ext<U[N], volatile U[]> : std::true_type {};

template <class U, size_t N>
struct is_pointer_compatible_ext<U[N], const volatile U[]> : std::true_type {};

#endif  // CXX_TOOLS_TYPE_TRAITS_H