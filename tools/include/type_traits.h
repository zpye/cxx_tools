#ifndef CXX_TOOLS_TYPE_TRAITS_H
#define CXX_TOOLS_TYPE_TRAITS_H

template< typename T >
struct func_traits;

template< typename R, typename... Args >
struct func_traits< R(Args...) >
{
    using return_type = R;
};

#endif // CXX_TOOLS_TYPE_TRAITS_H