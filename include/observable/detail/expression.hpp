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
class update_tag
{
public:
    //! Evaluate all expressions registered to be updated by this tag.
    //!
    //! \warning This method cannot be safely called concurrently.
    void eval_all() const { funs_->apply([](auto && f) { f(); }); }

    //! Destructor.
    virtual ~update_tag() { }

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

    template <typename ValueType, typename UpdateTag>
    friend class expression;
};

//! Expressions manage expression tree updates.
//!
//! \tparam ValueType The expression's result value type. This is what `get()`
//!                   returns.
//! \tparam UpdateTag An arbitrary type that will serve to allow globally updating
//!                   just a subset of expressions.
//! \warning None of the methods in this class can be safely called concurrently.
template <typename ValueType, typename UpdateTag>
class expression
{
    static_assert(std::is_base_of<update_tag, UpdateTag>::value,
                  "UpdateTag needs to be derived from observable::update_tag.");

public:
    //! Create a new expression from the root of an expression tree.
    //!
    //! \param root Expression tree root.
    //! \param tag Update tag to be used for globally updating the expression.
    expression(expression_node<ValueType> && root, UpdateTag const & tag) :
        root_ { std::move(root) },
        update_tag_ { tag }
    {
        expression_id_ = update_tag_.insert(this);
    }

    //! Evaluate the expression. This will ensure that the expression's result
    //! is up-to-date.
    void eval() { root_.eval(); }

    //! Get the expression's result. If update has not been called, the result
    //! might be stale.
    auto get() const { return root_.get(); }

    //! Destructor.
    ~expression() { update_tag_.remove(expression_id_); }

public:
    //! Expressions are default-constructible.
    expression() =default;

    //! Expressions are not copy-constructible.
    expression(expression const &) =delete;

    //! Expressions are not copy-assignable.
    auto operator=(expression const &) -> expression & =delete;

    //! Expressions are move-constructible.
    expression(expression &&) noexcept =default;

    //! Expressions are move-assignable.
    auto operator=(expression &&) noexcept -> expression & =default;

protected:
    //! Return the expression tree's root node.
    auto root_node() -> expression_node<ValueType> & { return root_; }

private:
    expression_node<ValueType> root_;
    UpdateTag update_tag_;
    typename UpdateTag::id expression_id_;
};

//! Update tag used for expressions that are updated immediately whenever
//! an expression node changes.
//!
//! Expressions tagged with this type do not need manual updates. Any manual
//! update calls will not do anything.
struct immediate_update_tag;

struct dummy_update_tag : update_tag { };

inline auto get_dummy_tag_()
{
    static dummy_update_tag dummy_tag;
    return dummy_tag;
}

//! Specialized expression that is updated immediately.
//!
//! \see expression<ValueType, UpdateTag>
template <typename ValueType>
class expression<ValueType, immediate_update_tag> :
    public expression<ValueType, dummy_update_tag>
{
public:
    explicit expression(expression_node<ValueType> && root) :
        expression<ValueType, dummy_update_tag>(std::move(root), get_dummy_tag_())
    {
        sub = root_node().subscribe([this]() { eval(); });
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
