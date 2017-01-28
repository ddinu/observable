#pragma once
#include <utility>
#include "observable/expression/utility.hpp"
#include "observable/expression/tree.hpp"

namespace observable { inline namespace expr {

// Unary operators.

#define OBSERVABLE_MAKE_UNARY_OP(OP) \
template <typename T> \
inline auto operator OP (T && arg) noexcept(noexcept(OP op::declval<T>())) \
    -> std::enable_if_t<op::is_observable<T>::value, \
                        expression_node<decltype(OP op::declval<T>())>> \
{ \
    return op::make_node([](auto && v) { return OP v; }, std::forward<T>(arg)); \
}

OBSERVABLE_MAKE_UNARY_OP(!)
OBSERVABLE_MAKE_UNARY_OP(~)
OBSERVABLE_MAKE_UNARY_OP(+)
OBSERVABLE_MAKE_UNARY_OP(-)

// Binary operators.

#define OBSERVABLE_MAKE_BINARY_OP(OP) \
template <typename A, typename B> \
inline auto operator OP (value<A> & a, B && b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> std::enable_if_t<!op::is_observable<B>::value, \
                        expression_node<decltype(op::declval<A>() OP op::declval<B>())>> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                         a, std::forward<B>(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (A && a, value<B> & b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> std::enable_if_t<!op::is_observable<A>::value, \
                        expression_node<decltype(op::declval<A>() OP op::declval<B>())>> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                         std::forward<A>(a), b); \
} \
\
template <typename A, typename B> \
inline auto operator OP (value<A> & a, value<B> & b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> expression_node<decltype(op::declval<A>() OP op::declval<B>())> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, a, b); \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, B && b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> std::enable_if_t<!op::is_observable<B>::value, \
                        expression_node<decltype(op::declval<A>() OP op::declval<B>())>> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                         std::move(a), std::forward<B>(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (A && a, expression_node<B> && b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> std::enable_if_t<!op::is_observable<A>::value, \
                        expression_node<decltype(op::declval<A>() OP op::declval<B>())>> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                         std::forward<A>(a), std::move(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, expression_node<B> && b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> expression_node<decltype(op::declval<A>() OP op::declval<B>())> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                         std::move(a), std::move(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (value<A> & a, expression_node<B> && b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> expression_node<decltype(op::declval<A>() OP op::declval<B>())> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                         a, std::move(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, value<B> & b) \
    noexcept(noexcept(op::declval<A>() OP op::declval<B>())) \
    -> expression_node<decltype(op::declval<A>() OP op::declval<B>())> \
{ \
    return op::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                         std::move(a), b); \
}

OBSERVABLE_MAKE_BINARY_OP(*)
OBSERVABLE_MAKE_BINARY_OP(/)
OBSERVABLE_MAKE_BINARY_OP(%)
OBSERVABLE_MAKE_BINARY_OP(+)
OBSERVABLE_MAKE_BINARY_OP(-)
OBSERVABLE_MAKE_BINARY_OP(<<)
OBSERVABLE_MAKE_BINARY_OP(>>)
OBSERVABLE_MAKE_BINARY_OP(<)
OBSERVABLE_MAKE_BINARY_OP(<=)
OBSERVABLE_MAKE_BINARY_OP(>)
OBSERVABLE_MAKE_BINARY_OP(>=)
OBSERVABLE_MAKE_BINARY_OP(==)
OBSERVABLE_MAKE_BINARY_OP(!=)
OBSERVABLE_MAKE_BINARY_OP(&)
OBSERVABLE_MAKE_BINARY_OP(^)
OBSERVABLE_MAKE_BINARY_OP(|)
OBSERVABLE_MAKE_BINARY_OP(&&)
OBSERVABLE_MAKE_BINARY_OP(||)

} }
