#pragma once
#include "observable/detail/expression_ops_utility.hpp"
#include "observable/detail/expression_tree.hpp"

namespace observable { inline namespace expr {

// Unary operators.

#define OBSERVABLE_MAKE_UNARY_OP(OP) \
template <typename T> \
inline auto operator OP (value<T> & arg) -> expression_node<decltype(OP op::var_type(arg))> \
{ \
    return { \
        [](auto && v) { return OP v; }, \
        expression_node<T> { arg } \
    }; \
} \
\
template <typename T> \
inline auto operator OP (expression_node<T> && arg) -> expression_node<decltype(OP op::var_type(arg))> \
{ \
    return { \
        [](auto && v) { return OP v; }, \
        std::move(arg) \
    }; \
}

OBSERVABLE_MAKE_UNARY_OP(!)
OBSERVABLE_MAKE_UNARY_OP(~)
OBSERVABLE_MAKE_UNARY_OP(+)
OBSERVABLE_MAKE_UNARY_OP(-)

// Binary operators.

#define OBSERVABLE_MAKE_BINARY_OP(OP) \
template <typename A, typename B> \
inline auto operator OP (value<A> & a, B && b) \
    -> std::enable_if_t<!op::is_observable<B>::value, \
                        expression_node<decltype(op::var_type(a) OP op::var_type(b))>> \
{ \
    return  { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        expression_node<A> { a }, \
        expression_node<std::decay_t<B>> { std::forward<B>(b) } \
    }; \
} \
\
template <typename A, typename B> \
inline auto operator OP (A && a, value<B> & b) \
    -> std::enable_if_t<!op::is_observable<A>::value, \
                        expression_node<decltype(op::var_type(a) OP op::var_type(b))>> \
{ \
    return { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        expression_node<std::decay_t<A>> { std::forward<A>(a) }, \
        expression_node<B> { b } \
    }; \
} \
\
template <typename A, typename B> \
inline auto operator OP (value<A> & a, value<B> & b) \
    -> expression_node<decltype(op::var_type(a) OP op::var_type(b))> \
{ \
    return { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        expression_node<A> { a }, \
        expression_node<B> { b } \
    }; \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, B && b) \
    -> std::enable_if_t<!op::is_observable<B>::value, \
                        expression_node<decltype(op::var_type(a) OP op::var_type(b))>> \
{ \
    return { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        std::move(a), \
        expression_node<std::decay_t<B>> { std::forward<B>(b) } \
    }; \
} \
\
template <typename A, typename B> \
inline auto operator OP (A && a, expression_node<B> && b) \
    -> std::enable_if_t<!op::is_observable<A>::value, \
                        expression_node<decltype(op::var_type(a) OP op::var_type(b))>> \
{ \
    return { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        expression_node<std::decay_t<A>> { std::forward<A>(a) }, \
        std::move(b) \
    }; \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, expression_node<B> && b) \
    -> expression_node<decltype(op::var_type(a) OP op::var_type(b))> \
{ \
    return { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        std::move(a), \
        std::move(b) \
    }; \
} \
\
template <typename A, typename B> \
inline auto operator OP (value<A> & a, expression_node<B> && b) \
    -> expression_node<decltype(op::var_type(a) OP op::var_type(b))> \
{ \
    return { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        expression_node<A> { a }, \
        std::move(b) \
    }; \
} \
\
template <typename A, typename B> \
inline auto operator OP (expression_node<A> && a, value<B> & b) \
    -> expression_node<decltype(op::var_type(a) OP op::var_type(b))> \
{ \
    return { \
        [](auto && av, auto && bv) { return av OP bv; }, \
        std::move(a), \
        expression_node<B> { b } \
    }; \
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
