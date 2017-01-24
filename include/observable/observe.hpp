#pragma once
#include <memory>
#include "observable/value.hpp"
#include "observable/expression/expression.hpp"
#include "observable/expression/operators.hpp"
#include "observable/expression/tree.hpp"

namespace observable {

//! Updates all values that were associated with this updater by providing the
//! updater as a first parameter to observe().
//!
//! This class can be used as-is or derived.
class updater : public expr::expression_evaluator
{
public:
    //! Update all values that have been associated with this updater.
    auto update() { eval_all(); }

private:
    using expr::expression_evaluator::eval_all;
};

//! Observe changes to a single value with automatic synchronization.
//!
//! Returns a value that is kept in-sync with the provided value.
//!
//! \param[in] val Value to observe.
//! \return A value that mirrors the provided parameter.
template <typename ValueType, typename ... Rest>
inline auto observe(value<ValueType, Rest ...> & val)
{
    using expression_type = expr::expression<ValueType, expr::immediate_evaluator>;
    auto e = std::make_unique<expression_type>(expr::expression_node<ValueType> { val });
    return value<ValueType> { std::move(e) };
}

//! Observe an expression with automatic evaluation.
//!
//! Returns a value that is updated whenever the provided expression changes.
//!
//! \param[in] root Expression to observe.
//! \return A value that is updated when the provided expression changes.
template <typename ValueType>
inline auto observe(expr::expression_node<ValueType> && root)
{
    using expression_type = expr::expression<ValueType, expr::immediate_evaluator>;
    auto e = std::make_unique<expression_type>(std::move(root));
    return value<ValueType> { std::move(e) };
}

//! Observe changes to a single value with manual synchronization.
//!
//! Returns a value that is synchronized with the provided value whenever the
//! provided updater is updated.
//!
//! \param[in] updater An updater to be used for synchronizing the returned value
//!                    to the provided value.
//! \param[in] val A value to synchronize with the returned value.
//! \return A value that is synchronized to the provided value.
template <typename UpdaterType, typename ValueType, typename ... Rest>
inline auto observe(UpdaterType & updater, value<ValueType, Rest ...> & val)
{
    using expression_type = expr::expression<ValueType, UpdaterType>;
    auto root = expr::expression_node<ValueType> { val };
    auto e = std::make_unique<expression_type>(std::move(root), updater);
    return value<ValueType> { std::move(e) };
}

//! Observe an expression with manual synchronization.
//!
//! Returns a value that is updated when the provided updater is updated, if
//! and only if the provided expression has changed.
//!
//! \param[in] updater An updater to be used for updating the returned value if
//!                    the expression changes.
//! \param[in] root An expression to be used for updating the returned value.
//! \return A value that is updated from the provided expression.
template <typename UpdaterType, typename ValueType>
inline auto observe(UpdaterType & updater, expr::expression_node<ValueType> && root)
{
    using expression_type = expr::expression<ValueType, UpdaterType>;
    auto e = std::make_unique<expression_type>(std::move(root), updater);
    return value<ValueType> { std::move(e) };
}

}
