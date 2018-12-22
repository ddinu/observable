#pragma once
#include <cassert>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <observable/subscription.hpp>
#include <observable/value.hpp>
#include <observable/expressions/tree.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable { inline namespace expr {

//! Expression evaluators can be used to manually evaluate multiple expressions at
//! the same time.
//!
//! You can use this type as-is or extend it.
//!
//! \ingroup observable_detail
class expression_evaluator
{
public:
    //! Evaluate all expressions associated with this instance.
    //!
    //! \note This method can be safely called in parallel, from multiple threads.
    void eval_all() const
    {
        std::lock_guard<std::mutex> const lock { data_->mutex };
        for(auto && p : data_->funs)
            p.second();
    }

private:
    // Expressions are inserted and removed in the order in which they are
    // created. Evaluating them in this order guarantees that the evaluation
    // order is correct (insert is called from the constructor, nothing can
    // have a dependency on the inserted expression yet).

    using id = void const *;

    //! Register a new expression to be evaluated by this evaluator.
    //!
    //! \return Instance id that can be used to unregister the expression.
    //! \note This method can be safely called in parallel, from multiple threads.
    template <typename ExpressionType>
    auto insert(ExpressionType * expr)
    {
        assert(expr);
        std::lock_guard<std::mutex> const lock { data_->mutex };

        data_->funs.emplace_back(expr, [=]() { expr->eval(); });
        return id { expr };
    }

    //! Unregister a previously registered expression.
    //!
    //! \param[in] instance_id Id that has been returned by `insert()`.
    //! \note This method can be safely called in parallel, from multiple threads.
    void remove(id instance_id)
    {
        std::lock_guard<std::mutex> const lock { data_->mutex };

        auto const it = find_if(begin(data_->funs),
                                end(data_->funs),
                                [&](auto && p) { return p.first == instance_id; });
        assert(it != end(data_->funs));
        if(it != end(data_->funs))
            data_->funs.erase(it);
    }

private:
    struct data {
        std::deque<std::pair<id, std::function<void()>>> funs;
        std::mutex mutex;
    };

    std::shared_ptr<data> data_ { std::make_shared<data>() };

    template <typename ValueType, typename UpdaterType>
    friend class expression;
};

//! Expressions manage expression tree evaluation and results.
//!
//! Expressions are also value updaters, so they can be used for updating a
//! value when an expression tree changes.
//!
//! \tparam ValueType The expression's result value type. This is what get()
//!                   returns.
//! \tparam EvaluatorType An instance of expression_evaluator, or a type derived
//!                       from it.
//! \warning None of the methods in this class can be safely called concurrently.
//!
//! \ingroup observable_detail
template <typename ValueType, typename EvaluatorType=expression_evaluator>
class expression : public value_updater<ValueType>
{
    static_assert(std::is_base_of<expression_evaluator, EvaluatorType>::value,
                  "EvaluatorType needs to be derived from expression_evaluator.");

public:
    //! Create a new expression from the root node of an expression tree.
    //!
    //! \param[in] root Expression tree root node.
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

    //! Retrieve the expression's result.
    //!
    //! \warning If eval() has not been called, the result might be stale.
    virtual auto get() const -> ValueType override { return root_.get(); }

    virtual void set_value_notifier(std::function<void(ValueType &&)> const & notifier) override
    {
        value_notifier_ = notifier;
    }

    //! Destructor.
    virtual ~expression() override { evaluator_.remove(expression_id_); }

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
//! Expressions using this evaluator do not need manual updates. Manual update
//! calls will not do anything.
//!
//! \ingroup observable_detail
struct immediate_evaluator final : expression_evaluator { };

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
//! \ingroup observable_detail
template <typename ValueType>
class expression<ValueType, immediate_evaluator> :
    public expression<ValueType, expression_evaluator>
{
public:
    //! Create a new expression from the root node of an expression tree.
    //!
    //! \param[in] root Expression tree root node.
    explicit expression(expression_node<ValueType> && root) :
        expression<ValueType, expression_evaluator>(std::move(root),
                                                    get_dummy_evaluator_())
    {
        sub = this->root_node().subscribe([&]() { this->eval(); });
    }

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

private:
    unique_subscription sub;
};

} }

OBSERVABLE_END_CONFIGURE_WARNINGS
