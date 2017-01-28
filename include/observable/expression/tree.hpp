#pragma once
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "observable/subject.hpp"
#include "observable/subscription.hpp"
#include "observable/value.hpp"

namespace observable { inline namespace expr {

//! An expression tree can evaluate an arbitrary expression.
//!
//! The term expression, is intentionally vague as it can be anything.
//!
//! Expressions are formed from n-ary, user-supplied operators and operands that
//! can be constants, observable values or other expression nodes.
//!
//! The tree will propagate change notifications upwards, so any change
//! to any value<> contained in the expression will be propagated to the root
//! node.
//!
//! When evaluating the root node, only nodes that have been changed will be
//! evaluated.
//!
//! \warning None of the class' methods can be safely called concurrently.
template <typename ResultType>
class expression_node
{
public:
    //! Create a new node from a constant value.
    //!
    //! \param[in] constant A constant value that will become the node's evaluated
    //!                     value.
    //!
    //! Nodes created with this constructor are called constant nodes. These nodes
    //! never notify their subscribers of value changes.
    template <typename ValueType>
    explicit expression_node(ValueType && constant)
    {
        static_assert(std::is_convertible<ValueType, ResultType>::value,
                      "ValueType must be convertible to ResultType.");

        data_->result = std::forward<ValueType>(constant);
        data_->dirty = false;
        data_->eval = []() { };
    }

    //! Create a new node from an observable value.
    //!
    //! \param[in] value An observable value who will become the node's evaluated
    //!                  value. This value does not need to be kept alive for any
    //!                  length of time.
    //!
    //! Nodes created with this constructor are called value nodes. These nodes
    //! notify their subscribers of value changes as long as the value is alive.
    template <typename ValueType, typename ... Rest>
    explicit expression_node(value<ValueType, Rest ...> & value)
    {
        static_assert(std::is_convertible<ValueType, ResultType>::value,
                      "ValueType must be convertible to ResultType.");

        data_->subs.emplace_back(value.subscribe([d = data_.get()]() {
                                                      d->dirty = true;
                                                      d->notify();
                                                  }));

        data_->eval = [v = &value, d = data_.get()]() {
                          if(!d->dirty)
                              return;

                          d->result = v->get();
                          d->dirty = false;
                      };

        data_->eval(); // So we cache the value in case the observable value dies
                       // before our node's first eval.
    }

    //! Create a new node from a number of nodes and an n-ary operation.
    //!
    //! \param[in] op An n-ary operation with the signature below:
    //!
    //!                     ResultType (ValueType const & ...)
    //!
    //! \param[in] nodes Expression nodes who's value will be the operand to the
    //!                  operation.
    //!
    //! Nodes created with this constructor are called unary nodes. These nodes
    //! notify their subscribers of value changes.
    template <typename OpType, typename ... ValueType>
    explicit expression_node(OpType && op, expression_node<ValueType> && ... nodes)
    {
        static_assert(std::is_convertible<decltype(op(ValueType {} ...)), ResultType>::value,
                      "Operation must return a type that is convertible to ResultType.");

        subscribe_to_nodes(nodes ...);

        data_->eval = [t = std::make_tuple(std::move(nodes) ...),
                       o = std::forward<OpType>(op),
                       d = data_.get()]() mutable {
                          if(!d->dirty)
                              return;

                          constexpr auto const size = std::tuple_size<decltype(t)>::value;
                          constexpr auto const indices = std::make_index_sequence<size> { };

                          d->result = call_with_tuple(o, t, indices);
                          d->dirty = false;
                      };

        data_->eval();
    }

    //! Execute the stored operation and update the node's result value.
    auto eval() const { data_->eval(); }

    //! Retrieve the expression node's result value. This will not evaluate the
    //! node so this value might be stale.
    auto get() const { return data_->result; }

    //! Subscribe to change notifications from this node.
    template <typename Observer>
    auto subscribe(Observer && callable) { return data_->subscribe(callable); }

public:
    //! Expression nodes are not default-constructible.
    expression_node() =default;

    //! Expression nodes are copy-constructible.
    //!
    //! \warning expression_nodes use shallow copies.
    expression_node(expression_node const &) =default;

    //! Expression nodes are copy-assignable.
    //!
    //! \warning expression_nodes use shallow copies.
    auto operator=(expression_node const &) -> expression_node & =default;

    //! Expression nodes are move-constructible.
    expression_node(expression_node && other) noexcept =default;

    //! Expression nodes are move-assignable.
    auto operator=(expression_node && other) noexcept -> expression_node & =default;

private:
    template <typename Head, typename ... Nodes>
    void subscribe_to_nodes(Head & head, Nodes & ... nodes)
    {
        data_->subs.emplace_back(
                    head.subscribe([d = data_.get()]() {
                                        d->dirty = true;
                                        d->notify();
                                    }));

        subscribe_to_nodes(nodes ...);
    }

    template <typename ... Nodes>
    void subscribe_to_nodes()
    {
        // Do nothing.
    }

    template <typename Tuple, std::size_t I=0>
    static auto eval_tuple(Tuple & nodes) ->
        typename std::enable_if<I < std::tuple_size<Tuple>::value, void>::type
    {
        std::get<I>(nodes).eval();
        eval_tuple<Tuple, I + 1>(nodes);
    }

    template <typename Tuple, std::size_t I=0>
    static auto eval_tuple(Tuple & nodes) ->
        typename std::enable_if<I >= std::tuple_size<Tuple>::value, void>::type
    {
        // Do nothing.
    }

    template <typename Fun, typename Tuple, std::size_t ... I>
    static auto call_with_tuple(Fun & fun, Tuple & nodes, std::index_sequence<I ...>)
    {
        eval_tuple(nodes);
        return fun(std::get<I>(nodes).get() ...);
    }

private:
    struct data : subject<void()>
    {
        ResultType result;
        bool dirty = true;
        std::function<void()> eval;
        std::vector<unique_subscription> subs;
    };

    std::shared_ptr<data> data_ { std::make_shared<data>() };
};

//! Check if a type is an expression node.
template <typename T>
struct is_expression_node_ : std::false_type { };

template <typename T>
struct is_expression_node_<expression_node<T>> : std::true_type { };

template <typename T>
struct is_expression_node : is_expression_node_<std::decay_t<T>> { };

} }
