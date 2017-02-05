#pragma once
#include <utility>
#include "observable/expressions/utility.hpp"
#include "observable/expressions/tree.hpp"

//! Create a unary operator.
//!
//! \ingroup observable_detail
#define OBSERVABLE_DEFINE_UNARY_OP(OP) \
template <typename T> \
inline auto operator OP (T && arg) noexcept(noexcept(OP expr_detail::declval<T>())) \
    -> std::enable_if_t<expr_detail::is_observable<T>::value, \
                        expression_node<decltype(OP expr_detail::declval<T>())>> \
{ \
    return expr_detail::make_node([](auto && v) { return OP v; }, std::forward<T>(arg)); \
}

//! Create a binary operator.
//!
//! \ingroup observable_detail
#define OBSERVABLE_DEFINE_BINARY_OP(OP) \
template <typename A, typename B, typename ... R> \
inline auto operator OP (value<A, R ...> & a, B && b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> std::enable_if_t<!expr_detail::is_observable<B>::value, \
                        expression_node<decltype(expr_detail::declval<A>() OP \
                                                 expr_detail::declval<B>())>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                                  a, std::forward<B>(b)); \
} \
\
template <typename A, typename B, typename ... R> \
inline auto operator OP (A && a, value<B, R ...> & b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> std::enable_if_t<!expr_detail::is_observable<A>::value, \
                        expression_node<decltype(expr_detail::declval<A>() OP \
                                                 expr_detail::declval<B>())>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                                  std::forward<A>(a), b); \
} \
\
template <typename A, typename Eq1, typename B, typename Eq2> \
inline auto operator OP (value<A, Eq1> & a, value<B, Eq2> & b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> expression_node<decltype(expr_detail::declval<A>() OP expr_detail::declval<B>())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, a, b); \
} \
\
template <typename A, typename Eq1, typename Enc1, \
          typename B, typename Eq2, typename Enc2> \
inline auto operator OP (value<A, Eq1, Enc1> & a, value<B, Eq2, Enc2> & b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> expression_node<decltype(expr_detail::declval<A>() OP expr_detail::declval<B>())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, a, b); \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, B && b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> std::enable_if_t<!expr_detail::is_observable<B>::value, \
                        expression_node<decltype(expr_detail::declval<A>() OP \
                                                 expr_detail::declval<B>())>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                                  std::move(a), std::forward<B>(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (A && a, expression_node<B> && b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> std::enable_if_t<!expr_detail::is_observable<A>::value, \
                        expression_node<decltype(expr_detail::declval<A>() OP \
                                                 expr_detail::declval<B>())>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                                  std::forward<A>(a), std::move(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, expression_node<B> && b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> expression_node<decltype(expr_detail::declval<A>() OP \
                                expr_detail::declval<B>())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                                  std::move(a), std::move(b)); \
} \
\
template <typename A, typename B, typename ... R> \
inline auto operator OP (value<A, R ...> & a, expression_node<B> && b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> expression_node<decltype(expr_detail::declval<A>() OP expr_detail::declval<B>())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                                  a, std::move(b)); \
} \
\
template <typename A, typename B, typename ... R> \
inline auto operator OP (expression_node<A> && a, value<B, R ...> & b) \
    noexcept(noexcept(expr_detail::declval<A>() OP expr_detail::declval<B>())) \
    -> expression_node<decltype(expr_detail::declval<A>() OP expr_detail::declval<B>())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return av OP bv; }, \
                                  std::move(a), b); \
}

namespace observable { inline namespace expr {

// Unary operators.

OBSERVABLE_DEFINE_UNARY_OP(!)
OBSERVABLE_DEFINE_UNARY_OP(~)
OBSERVABLE_DEFINE_UNARY_OP(+)
OBSERVABLE_DEFINE_UNARY_OP(-)

// Binary operators.

OBSERVABLE_DEFINE_BINARY_OP(*)
OBSERVABLE_DEFINE_BINARY_OP(/)
OBSERVABLE_DEFINE_BINARY_OP(%)
OBSERVABLE_DEFINE_BINARY_OP(+)
OBSERVABLE_DEFINE_BINARY_OP(-)
OBSERVABLE_DEFINE_BINARY_OP(<<)
OBSERVABLE_DEFINE_BINARY_OP(>>)
OBSERVABLE_DEFINE_BINARY_OP(<)
OBSERVABLE_DEFINE_BINARY_OP(<=)
OBSERVABLE_DEFINE_BINARY_OP(>)
OBSERVABLE_DEFINE_BINARY_OP(>=)
OBSERVABLE_DEFINE_BINARY_OP(==)
OBSERVABLE_DEFINE_BINARY_OP(!=)
OBSERVABLE_DEFINE_BINARY_OP(&)
OBSERVABLE_DEFINE_BINARY_OP(^)
OBSERVABLE_DEFINE_BINARY_OP(|)
OBSERVABLE_DEFINE_BINARY_OP(&&)
OBSERVABLE_DEFINE_BINARY_OP(||)

} }
