#include <type_traits>
#include "observable/value.hpp"
#include "observable/expression/tree.hpp"

namespace observable { inline namespace expr { namespace op {

// Check for values.

template <typename T>
struct is_value_ { static constexpr auto const value = false; };

template <typename T>
struct is_value_<value<T>> { static constexpr auto const value = true; };

template <typename T>
struct is_value : is_value_<std::decay_t<T>> { };

// Check for expression_nodes.

template <typename T>
struct is_expression_node_ { static constexpr auto const value = false; };

template <typename T>
struct is_expression_node_<expression_node<T>> { static constexpr auto const value = true; };

template <typename T>
struct is_expression_node : is_expression_node_<std::decay_t<T>> { };

// Check for expressions or values.

template <typename T>
struct is_observable
{
    static constexpr auto const value = is_value<T>::value || is_expression_node<T>::value;
};

// Value extractors for value<> or expression_node<>.

template <typename T>
struct val_ { using type = T; };

template <typename T>
struct val_<value<T>> { using type = T; };

template <typename T>
struct val_<expression_node<T>> { using type = T; };

template <typename T>
struct val_type : val_<std::decay_t<T>> { };

template <typename T>
using val_type_t = typename val_type<T>::type;

template <typename T>
auto var_type(T &&) -> val_type_t<T>;

} } }
