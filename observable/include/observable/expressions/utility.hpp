#pragma once
#include <type_traits>
#include <utility>
#include <observable/value.hpp>
#include <observable/expressions/tree.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable { inline namespace expr { namespace expr_detail {

//! Check if a type is either an expression_node or an observable
//! value<ValueType, EqualityComparator>.
//!
//! The static member ``value`` will be true if the provided type is either an
//! observable value<ValueType, EqualityComparator> or an
//! expression_node.
//!
//! \ingroup observable_detail
template <typename T>
struct is_observable :
    std::integral_constant<bool, is_value<T>::value ||
                                 is_expression_node<T>::value>
{ };

//! Check if any of the provided types are observable.
//!
//! The static member ``value`` will be true if at least one of the provided types
//! is an observable value<ValueType, EqualityComparator> or an expression_node.
//!
//! \ingroup observable_detail
template <typename ... T>
struct are_any_observable;

//! \cond
template <typename H, typename ... T>
struct are_any_observable<H, T ...> :
    std::integral_constant<bool, are_any_observable<H>::value ||
                                 are_any_observable<T ...>::value>
{ };

template <typename T>
struct are_any_observable<T> : is_observable<T>
{ };
//! \endcond

//! \cond
template <typename T>
struct val_type_ { using type = T; };

template <typename T, typename ... R>
struct val_type_<value<T, R ...>> { using type = T; };

template <typename T>
struct val_type_<expression_node<T>> { using type = T; };
//! \endcond

//! Extract the value type from an expression_node or observable
//! value<ValueType, EqualityComparator>.
//!
//! \ingroup observable_detail
template <typename T>
struct val_type : val_type_<std::decay_t<T>> { };

//! Convenience typedef for extracting the value type from an expression_node or
//! observable value<ValueType, EqualityComparator>.
//!
//! \see val_type
//! \ingroup observable_detail
template <typename T>
using val_type_t = typename val_type<T>::type;

//! Computes the type of the expression_node created for an expression with
//! callable ``Op`` and corresponding arguments.
//!
//! \ingroup observable_detail
template <typename Op, typename ... Args>
struct result_node
{
#if defined(__cpp_lib_invoke) && __cpp_lib_invoke && defined(_HAS_CXX17) && _HAS_CXX17
    using type = expression_node<
                    std::decay_t<
                        std::invoke_result_t<
                            std::decay_t<Op>, val_type_t<Args> ...>>>;
#else
    using type = expression_node<
                    std::decay_t<
                        std::result_of_t<
                            std::decay_t<Op>(val_type_t<Args> ...)>>>;
#endif
};

//! Type of the expression_node created for an expression with callable ``Op`` and
//! corresponding arguments.
//!
//! \ingroup observable_detail
template <typename Op, typename ... Args>
using result_node_t = typename result_node<Op, Args ...>::type;

//! Create a node from a regular type.
//!
//! \ingroup observable_detail
template <typename T>
inline auto make_node(T && val)
{
    return expression_node<val_type_t<T>> { std::forward<T>(val) };
}

//! Create a node from an observable value reference.
//!
//! \ingroup observable_detail
template <typename T, typename ... R>
inline auto make_node(value<T, R ...> & val)
{
    return expression_node<T> { val };
}

//! Create a node from a temporary expression_node.
//!
//! \ingroup observable_detail
template <typename T>
inline auto make_node(expression_node<T> && node)
{
    return std::move(node);
}

//! Create a node from an operator and an arbitrary number of arguments.
//!
//! \ingroup observable_detail
template <typename Op, typename ... Args>
inline auto make_node(Op && op, Args && ... args)
{
    return result_node_t<Op, Args ...> {
        std::forward<Op>(op),
        make_node(std::forward<Args>(args)) ...
    };
}

} } }

OBSERVABLE_END_CONFIGURE_WARNINGS
