#pragma once
#include <cassert>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include "observable/subscription.hpp"
#include "observable/value.hpp"
#include "observable/detail/collection.hpp"
#include "observable/expression/tree.hpp"

namespace observable { inline namespace expr {

//! Expression evaluators can be used to manually evaluate multiple expressions at
//! the same time.
//!
//! You can use this type as-is or extend it.
class expression_evaluator
{
public:
    //! Evaluate all expressions associated with this evaluator instance.
    //!
    //! \warning This method cannot be safely called concurrently.
    void eval_all() const { funs_->apply([](auto && f) { f(); }); }

    //! Destructor.
    virtual ~expression_evaluator() { }

private:
    using eval_collection = detail::collection<std::function<void()>>;
    using id = eval_collection::id;

    //! Register a new expression to be evaluated by this evaluator.
    //!
    //! \return Instance id that can be used to unregister the expression.
    //! \note This method can be safely called in parallel, from multiple threads.
    template <typename ExpressionType>
    auto insert(ExpressionType * expr)
    {
        assert(expr);
        return funs_->insert([=]() { expr->eval(); });
    }

    //! Unregister a previously registered expression.
    //!
    //! \param[in] instance_id Id that has been returned by `insert()`.
    //! \note This method can be safely called in parallel, from multiple threads.
    void remove(id instance_id) { funs_->remove(instance_id); }

private:
    std::shared_ptr<eval_collection> funs_ { std::make_shared<eval_collection>() };

    template <typename ValueType, typename UpdaterType>
    friend class expression;
};

//! Expressions manage expression tree updates.
//!
//! \tparam ValueType The expression's result value type. This is what `get()`
//!                   returns.
//! \tparam EvaluatorType An instance of expression_evaluator or a type derived
//!                       from it.
//! \warning None of the methods in this class can be safely called concurrently.
template <typename ValueType, typename EvaluatorType=expression_evaluator>
class expression : public value_updater<ValueType>
{
    static_assert(std::is_base_of<expression_evaluator, EvaluatorType>::value,
                  "EvaluatorType needs to be derived from expression_evaluator.");

public:
    //! Create a new expression from the root of an expression tree.
    //!
    //! \param[in] root Expression tree root.
    //! \param[in] evaluator Expression evaluator to be used for globally updating
    //!                      the expression.
    expression(expression_node<ValueType> && root,
               EvaluatorType const & evaluator) :
        root_ { std::move(root) },
        evaluator_ { evaluator }
    {
        expression_id_ = evaluator_.insert(this);
    }

    //! Evaluate the expression. This will ensure that the expression's result
    //! is up-to-date.
    void eval()
    {
        root_.eval();
        value_notifier_(root_.get());
    }

    //! Get the expression's result.
    //!
    //! If eval() has not been called, the result might be stale.
    //!
    //! \see value_updater<ValueType>::get
    virtual auto get() const -> ValueType override { return root_.get(); }

    //! \see value_updater<ValueType>::set_value_notifier
    virtual void set_value_notifier(std::function<void(ValueType &&)> const & notifier) override
    {
        value_notifier_ = notifier;
    }

    //! Destructor.
    virtual ~expression() { evaluator_.remove(expression_id_); }

public:
    //! Expressions are default-constructible.
    expression() =default;

    //! Expressions are not copy-constructible.
    expression(expression const &) =delete;

    //! Expressions are not copy-assignable.
    auto operator=(expression const &) -> expression & =delete;

    //! Expressions are move-constructible.
    expression(expression &&) =default;

    //! Expressions are move-assignable.
    auto operator=(expression &&) -> expression & =default;

protected:
    //! Return the expression tree's root node.
    auto root_node() -> expression_node<ValueType> & { return root_; }

private:
    expression_node<ValueType> root_;
    EvaluatorType evaluator_;
    typename EvaluatorType::id expression_id_;
    std::function<void(ValueType &&)> value_notifier_ { [](auto &&) { } };
};

//! Evaluator used for expressions that are updated immediately, whenever an
//! expression node changes.
//!
//! Expressions associated with this type do not need manual updates. Any manual
//! update calls will not do anything.
struct immediate_evaluator : expression_evaluator { };

//! \cond
inline auto get_dummy_evaluator_()
{
    static expression_evaluator ev;
    return ev;
}
//! \endcond

//! Specialized expression that is updated immediately, whenever an expression
//! node changes.
//!
//! \see expression<ValueType, EvaluatorType>
template <typename ValueType>
class expression<ValueType, immediate_evaluator> :
    public expression<ValueType, expression_evaluator>
{
public:
    //! Create a new expression from an expression node.
    //!
    //! \param[in] root Expression node that is the root of an expression tree.
    explicit expression(expression_node<ValueType> && root) :
        expression<ValueType, expression_evaluator>(std::move(root),
                                                    get_dummy_evaluator_())
    {
        sub = this->root_node().subscribe([&]() { this->eval(); });
    }

public:
    expression() =delete;

    expression(expression const &) =delete;

    auto operator=(expression const &) -> expression & =delete;

    expression(expression &&) =default;

    auto operator=(expression &&) -> expression & =default;

private:
    unique_subscription sub;
};

} }
