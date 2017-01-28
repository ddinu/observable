#include <type_traits>
#include <utility>
#include "observable/value.hpp"
#include "observable/expression/tree.hpp"

namespace observable { inline namespace expr { namespace op {

// Check for expressions or values.

template <typename T>
struct is_observable :
    std::integral_constant<bool, is_value<T>::value ||
                                 is_expression_node<T>::value>
{ };

// Value extractors for value<> or expression_node<>.

template <typename T>
struct val_type_ { using type = T; };

template <typename T>
struct val_type_<value<T>> { using type = T; };

template <typename T>
struct val_type_<expression_node<T>> { using type = T; };

template <typename T>
struct val_type : val_type_<std::decay_t<T>> { };

template <typename T>
using val_type_t = typename val_type<T>::type;

template <typename T>
inline auto declval() -> val_type_t<T>;

// Node factory.

template <typename T>
inline auto make_node(T && val)
{
    return expression_node<val_type_t<T>> { std::forward<T>(val) };
}

template <typename T>
inline auto make_node(value<T> & val)
{
    return expression_node<T> { val };
}

template <typename T>
inline auto make_node(expression_node<T> && node)
{
    return std::move(node);
}

template <typename Op, typename Arg>
inline auto make_node(Op && op, Arg && arg)
{
    using result_type = std::decay_t<std::result_of_t<Op(val_type_t<Arg>)>>;
    return expression_node<result_type> {
        std::forward<Op>(op),
        make_node(std::forward<Arg>(arg))
    };
}

template <typename Op, typename A, typename B>
inline auto make_node(Op && op, A && a, B && b)
{
    using result_type = std::decay_t<std::result_of_t<Op(val_type_t<A>, val_type_t<B>)>>;
    return expression_node<result_type> {
        std::forward<Op>(op),
        make_node(std::forward<A>(a)),
        make_node(std::forward<B>(b))
    };
}

} } }
