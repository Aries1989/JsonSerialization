#ifndef CBIM_TYPE_TRAITS_HPP
#define CBIM_TYPE_TRAITS_HPP

#include <type_traits>
namespace cbim
{

#if __cplusplus >= 201703L

template<typename T, typename ...Types>
struct is_any_of: std::disjunction<std::is_same<T, Types>...>
{};

#else

template<
        typename T
        ,typename A1
        ,typename A2 = void
        ,typename A3 = void
        ,typename A4 = void
        ,typename A5 = void
        ,typename A6 = void
        ,typename A7 = void
        ,typename A8 = void
        >
struct is_any_of: std::integral_constant<
        bool
        ,  std::is_same<T, A1>::value
        || std::is_same<T, A2>::value
        || std::is_same<T, A3>::value
        || std::is_same<T, A4>::value
        || std::is_same<T, A5>::value
        || std::is_same<T, A6>::value
        || std::is_same<T, A7>::value
        || std::is_same<T, A8>::value
        >
{};

#endif

template<typename T, typename... Types>
struct enable_if_is_any_of
    :std::enable_if<is_any_of<T, Types...>::value>
{};

template<typename T, typename... Types>
struct disable_if_is_any_of
    :std::enable_if<!is_any_of<T, Types...>::value>
{};

#define __CBIM_ENABLE_IF_IS_ANY_OF(T, ...) \
    typename ::cbim::enable_if_is_any_of<T, __VA_ARGS__>::type* = 0

#define __CBIM_DISABLE_IF_IS_ANY_OF(T, ...) \
    typename ::cbim::disable_if_is_any_of<T, __VA_ARGS__>::type* = 0

}
#endif
