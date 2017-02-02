#pragma once
#include <memory>
#include <type_traits>
#include "observable/value.hpp"
#include "observable/expressions/expression.hpp"
#include "observable/expressions/operators.hpp"
#include "observable/expressions/tree.hpp"

namespace observable {

//! Update all observable values that were associated with an updater instance.
//!
//! You associate an observable value with an updater instance by providing the
//! updater as a first parameter to ``observe()``.
//!
//! \note This class can be used as-is or derived.
class updater : public expr::expression_evaluator
{
public:
    //! Update all observable values that have been associated with this instance.
    void update() { eval_all(); }

private:
    using expr::expression_evaluator::eval_all;
};

//! Observe changes to a single value with automatic synchronization.
//!
//! Returns an observable value that is kept in-sync with the provided value.
//!
//! \param[in] val Value to observe.
//! \return An observable value that automatically mirrors the provided parameter.
template <typename ValueType, typename ... Rest>
inline auto observe(value<ValueType, Rest ...> & val)
{
    using expression_type = expr::expression<ValueType, expr::immediate_evaluator>;
    auto e = std::make_unique<expression_type>(expr::expression_node<ValueType> { val });
    return value<ValueType> { std::move(e) };
}

//! Observe changes to an expression tree with automatic evaluation.
//!
//! Returns a value that is updated whenever the provided expression tree changes.
//!
//! \param[in] root Expression tree to observe.
//! \return An observable value that is automatically updated when the provided
//!         expression tree changes.
template <typename ValueType>
inline auto observe(expr::expression_node<ValueType> && root)
{
    using expression_type = expr::expression<ValueType, expr::immediate_evaluator>;
    auto e = std::make_unique<expression_type>(std::move(root));
    return value<ValueType> { std::move(e) };
}

//! Observe changes to a single value with manual synchronization.
//!
//! Returns an observable value that is synchronized with the provided value
//! whenever the ``update()`` method is called on the provided \ref updater.
//!
//! \param[in] ud An \ref updater instance to be used for manually synchronizing
//!               the returned value to the provided value.
//! \param[in] val A value to synchronize with the returned value.
//! \return An observable value that is manually synchronized to the provided
//!         value.
template <typename UpdaterType, typename ValueType, typename ... Rest>
inline auto observe(UpdaterType & ud, value<ValueType, Rest ...> & val)
{
    static_assert(std::is_base_of<updater, UpdaterType>::value,
                  "UpdaterType must derive from updater.");

    using expression_type = expr::expression<ValueType, UpdaterType>;
    auto root = expr::expression_node<ValueType> { val };
    auto e = std::make_unique<expression_type>(std::move(root), ud);
    return value<ValueType> { std::move(e) };
}

//! Observe changes to an expression tree with manual synchronization.
//!
//! Returns an observable value that is updated when the ``update()`` method is
//! called on the provided \ref updater.
//!
//! \note The expression tree will only be evaluated if there have been changes
//!       since the last update() call.
//!
//! \param[in] ud An \ref updater instance to be used for manually updating the
//!               returned value with the expression tree.
//! \param[in] root An expression tree to be used for updating the returned value.
//! \return An observable value that is updated from the provided expression.
template <typename UpdaterType, typename ValueType>
inline auto observe(UpdaterType & ud, expr::expression_node<ValueType> && root)
{
    static_assert(std::is_base_of<updater, UpdaterType>::value,
                  "UpdaterType must derive from updater.");

    using expression_type = expr::expression<ValueType, UpdaterType>;
    auto e = std::make_unique<expression_type>(std::move(root), ud);
    return value<ValueType> { std::move(e) };
}

}
