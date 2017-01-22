#pragma once
#include <cassert>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include "observable/subscription.hpp"
#include "observable/detail/collection.hpp"
#include "observable/detail/expression_tree.hpp"

namespace observable { namespace detail {

//! Extend this type to create a custom update tag.
//!
//! An update tag will allow you to group expressions so that they can be upgraded
//! globally for each group.
class expression_updater
{
public:
    //! Evaluate all expressions registered to be updated by this tag.
    //!
    //! \warning This method cannot be safely called concurrently.
    void eval_all() const { funs_->apply([](auto && f) { f(); }); }

    //! Destructor.
    virtual ~expression_updater() { }

private:
    using eval_collection = collection<std::function<void()>>;
    using id = eval_collection::id;

    //! Register a new expression to be updated by this tag.
    //!
    //! \return Id that can be used to unregister the expression.
    //! \note This method can be safely called in parallel, from multiple threads.
    template <typename ExpressionType>
    auto insert(ExpressionType * expr)
    {
        assert(expr);
        return funs_->insert([=]() { expr->eval(); });
    }

    //! Unregister a previously registered expression from being updated by this
    //! tag.
    //!
    //! \param i Id that has been returned by `insert()`.
    //! \note This method can be safely called in parallel, from multiple threads.
    void remove(id i) { funs_->remove(i); }

private:
    std::shared_ptr<eval_collection> funs_ { std::make_shared<eval_collection>() };

    template <typename ValueType, typename UpdaterType>
    friend class expression;
};

//! Expressions manage expression tree updates.
//!
//! \tparam ValueType The expression's result value type. This is what `get()`
//!                   returns.
//! \tparam UpdaterType An arbitrary type that will serve to allow globally
//!                     updating just a subset of expressions.
//! \warning None of the methods in this class can be safely called concurrently.
template <typename ValueType, typename UpdaterType>
class expression : public value_updater<ValueType>
{
    static_assert(std::is_base_of<expression_updater, UpdaterType>::value,
                  "UpdaterType needs to be derived from observable::expression_updater.");

public:
    //! Create a new expression from the root of an expression tree.
    //!
    //! \param root Expression tree root.
    //! \param tag Update tag to be used for globally updating the expression.
    expression(expression_node<ValueType> && root, UpdaterType const & tag) :
        root_ { std::move(root) },
        update_tag_ { tag }
    {
        expression_id_ = update_tag_.insert(this);
    }

    //! Evaluate the expression. This will ensure that the expression's result
    //! is up-to-date.
    void eval()
    {
        root_.eval();
        value_notifier_(root_.get());
    }

    //! Get the expression's result. If update has not been called, the result
    //! might be stale.
    //!
    //! \see value_updater<ValueType>::get
    virtual auto get() const -> ValueType override { return root_.get(); }

    //! \see value_updater<ValueType>::set_value_notifier
    virtual void set_value_notifier(std::function<void(ValueType &&)> const & notifier) override
    {
        value_notifier_ = notifier;
    }

    //! Destructor.
    virtual ~expression() { update_tag_.remove(expression_id_); }

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
    UpdaterType update_tag_;
    typename UpdaterType::id expression_id_;
    std::function<void(ValueType &&)> value_notifier_ { [](auto &&) { } };
};

//! Update tag used for expressions that are updated immediately whenever
//! an expression node changes.
//!
//! Expressions tagged with this type do not need manual updates. Any manual
//! update calls will not do anything.
struct immediate_update_tag;

struct dummy_update_tag : expression_updater { };

inline auto get_dummy_tag_()
{
    static dummy_update_tag dummy_tag;
    return dummy_tag;
}

//! Specialized expression that is updated immediately.
//!
//! \see expression<ValueType, UpdaterType>
template <typename ValueType>
class expression<ValueType, immediate_update_tag> :
    public expression<ValueType, dummy_update_tag>
{
public:
    explicit expression(expression_node<ValueType> && root) :
        expression<ValueType, dummy_update_tag>(std::move(root), get_dummy_tag_())
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
