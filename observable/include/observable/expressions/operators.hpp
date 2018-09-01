#pragma once
#include <utility>
#include <observable/expressions/utility.hpp>
#include <observable/expressions/tree.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

//! Create a unary operator.
//!
//! \ingroup observable_detail
#define OBSERVABLE_DEFINE_UNARY_OP(OP) \
template <typename ... T> \
inline auto operator OP (value<T ...> & arg) \
    noexcept(noexcept(OP arg.get())) \
    -> expression_node<std::decay_t<decltype(OP arg.get())>> \
{ \
    return expr_detail::make_node([](auto && v) { return (OP v); }, arg); \
} \
\
template <typename T> \
inline auto operator OP (expression_node<T> && arg) \
    noexcept(noexcept(OP arg.get())) \
    -> expression_node<std::decay_t<decltype(OP arg.get())>> \
{ \
    return expr_detail::make_node([](auto && v) { return (OP v); }, \
                                  std::move(arg)); \
}

//! Create a binary operator.
//!
//! \ingroup observable_detail
#define OBSERVABLE_DEFINE_BINARY_OP(OP) \
template <typename B, typename ... A> \
inline auto operator OP (value<A ...> & a, B && b) \
    noexcept(noexcept(a.get() OP b)) \
    -> std::enable_if_t<!expr_detail::is_observable<B>::value, \
                        expression_node<decltype(a.get() OP b)>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  a, std::forward<B>(b)); \
} \
\
template <typename A, typename ... B> \
inline auto operator OP (A && a, value<B ...> & b) \
    noexcept(noexcept(a OP b.get())) \
    -> std::enable_if_t<!expr_detail::is_observable<A>::value, \
                        expression_node<decltype(a OP b.get())>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  std::forward<A>(a), b); \
} \
\
template <typename A, typename B> \
inline auto operator OP (value<A> & a, value<B> & b) \
    noexcept(noexcept(a.get() OP b.get())) \
    -> expression_node<decltype(a.get() OP b.get())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  a, b); \
} \
\
template <typename A, typename Enc1, \
          typename B, typename Enc2> \
inline auto operator OP (value<A, Enc1> & a, value<B, Enc2> & b) \
    noexcept(noexcept(a.get() OP b.get())) \
    -> expression_node<decltype(a.get() OP b.get())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  a, b); \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, B && b) \
    noexcept(noexcept(a.get() OP b)) \
    -> std::enable_if_t<!expr_detail::is_observable<B>::value, \
                        expression_node<decltype(a.get() OP b)>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  std::move(a), std::forward<B>(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (A && a, expression_node<B> && b) \
    noexcept(noexcept(a OP b.get())) \
    -> std::enable_if_t<!expr_detail::is_observable<A>::value, \
                        expression_node<decltype(a OP b.get())>> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  std::forward<A>(a), std::move(b)); \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, expression_node<B> && b) \
    noexcept(noexcept(a.get() OP b.get())) \
    -> expression_node<decltype(a.get() OP b.get())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  std::move(a), std::move(b)); \
} \
\
template <typename B, typename ... A> \
inline auto operator OP (value<A ...> & a, expression_node<B> && b) \
    noexcept(noexcept(a.get() OP b.get())) \
    -> expression_node<decltype(a.get() OP b.get())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
                                  a, std::move(b)); \
} \
\
template <typename A, typename ... B> \
inline auto operator OP (expression_node<A> && a, value<B ...> & b) \
    noexcept(noexcept(a.get() OP b.get())) \
    -> expression_node<decltype(a.get() OP b.get())> \
{ \
    return expr_detail::make_node([](auto && av, auto && bv) { return (av OP bv); }, \
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

OBSERVABLE_END_CONFIGURE_WARNINGS
