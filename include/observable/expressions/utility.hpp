#include <type_traits>
#include <utility>
#include "observable/value.hpp"
#include "observable/expressions/tree.hpp"

namespace observable { inline namespace expr { namespace op {

//! Check if a type is either an expression_node or an observable
//! value<ValueType, EqualityComparator>.
//!
//! The static member ``value`` will be true if the provided type is either an
//! observable value<ValueType, EqualityComparator> or an
//! expression_node.
template <typename T, typename ... R>
struct is_observable :
    std::integral_constant<bool, is_value<T, R ...>::value ||
                                 is_expression_node<T>::value>
{ };

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
template <typename ... T>
struct val_type : val_type_<std::decay_t<T> ...> { };

//! Convenience typedef for extracting the value type from an expression_node or
//! observable value<ValueType, EqualityComparator>.
//!
//! \see val_type
template <typename ... T>
using val_type_t = typename val_type<T ...>::type;

//! Function that can be used in an unevaluated context to extract a value type
//! from an expression_node or from an observable value<ValueType, EqualityComparator>.
//!
//! This is similar to ``std::declval()``.
template <typename ... T>
inline auto declval() -> val_type_t<T ...>;

//! Create a node from a regular type.
template <typename T>
inline auto make_node(T && val)
{
    return expression_node<val_type_t<T>> { std::forward<T>(val) };
}

//! Create a node from an observable value reference.
template <typename T, typename ... R>
inline auto make_node(value<T, R ...> & val)
{
    return expression_node<T> { val };
}

//! Create a node from a temporary expression_node.
template <typename T>
inline auto make_node(expression_node<T> && node)
{
    return std::move(node);
}

//! Create a node from an operator and an arbitrary number of arguments.
template <typename Op, typename ... Args>
inline auto make_node(Op && op, Args && ... args)
{
    using result_type = std::decay_t<std::result_of_t<Op(val_type_t<Args> ...)>>;
    return expression_node<result_type> {
        std::forward<Op>(op),
        make_node(std::forward<Args>(args)) ...
    };
}

} } }
