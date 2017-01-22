#pragma once
#include <memory>
#include "observable/value.hpp"
#include "observable/detail/expression.hpp"
#include "observable/detail/expression_ops.hpp"
#include "observable/detail/expression_tree.hpp"

namespace observable {

using updater = detail::expression_updater;

//! Observe changes to a single value with automatic synchronization.
//!
//! Returns a value that is kept in-sync with the provided value.
//!
//! \param val Value to observe.
//! \return A value that mirrors the provided parameter.
template <typename ValueType, typename ... Rest>
inline auto observe(value<ValueType, Rest ...> & val)
{
    using expression_type = detail::expression<ValueType, detail::immediate_update_tag>;
    auto expr = std::make_unique<expression_type>(expression_node<ValueType> { val });
    return value<ValueType> { std::move(expr) };
}

//! Observe an expression with automatic evaluation.
//!
//! Returns a value that is updated whenever the provided expression changes.
//!
//! \param root Expression to observe.
//! \return A value that is updated when the provided expression changes.
template <typename ValueType>
inline auto observe(expression_node<ValueType> && root)
{
    using expression_type = detail::expression<ValueType, detail::immediate_update_tag>;
    auto expr = std::make_unique<expression_type>(std::move(root));
    return value<ValueType> { std::move(expr) };
}

//! Observe changes to a single value with manual synchronization.
//!
//! Returns a value that is synchronized with the provided value whenever the
//! updater is evaluated.
//!
//! \param updater An updater to be used for synchronizing the returned value
//!                to the provided value.
//! \param val A value to synchronize with the returned value.
//! \return A value that is synchronized to the provided value.
template <typename UpdaterType, typename ValueType, typename ... Rest>
inline auto observe(UpdaterType & updater, value<ValueType, Rest ...> & val)
{
    using expression_type = detail::expression<ValueType, UpdaterType>;
    auto root = expression_node<ValueType> { val };
    auto expr = std::make_unique<expression_type>(std::move(root), updater);
    return value<ValueType> { std::move(expr) };
}

//! Observe an expression with manual evaluation.
//!
//! Returns a value that is updated when the provided updater is evaluated, if
//! and only if the provided expression has changed.
//!
//! \param updater An updater to be used for updating the returned value if
//!                the expression changes.
//! \param root An expression to be used for updating the returned value.
//! \return A value that is updated from the provided expression.
template <typename UpdaterType, typename ValueType>
inline auto observe(UpdaterType & updater, expression_node<ValueType> && root)
{
    using expression_type = detail::expression<ValueType, UpdaterType>;
    auto expr = std::make_unique<expression_type>(std::move(root), updater);
    return value<ValueType> { std::move(expr) };
}

}
