#ifndef CXX_TOOLS_TYPE_TRAITS_H
#define CXX_TOOLS_TYPE_TRAITS_H

#include <type_traits>
#include <cstdint>

template< typename T >
struct func_traits;

template< typename R, typename... Args >
struct func_traits< R(Args...) >
{
    using return_type = R;
};

// from C++ head file "memory" in visual studio
template <class U, class T>
struct is_convertible_ext : std::is_convertible<U*, T*>::type {};

template <class U, class T>
struct is_convertible_ext<U, T[]> : std::is_convertible<U(*)[], T(*)[]>::type {};

template <class U, class T, size_t N>
struct is_convertible_ext<U, T[N]> : std::is_convertible<U(*)[N], T(*)[N]>::type {};

#endif // CXX_TOOLS_TYPE_TRAITS_H