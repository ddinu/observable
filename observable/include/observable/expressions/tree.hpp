#pragma once
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <observable/subject.hpp>
#include <observable/subscription.hpp>
#include <observable/value.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable { inline namespace expr {

//! \cond
template <typename T>
struct is_expression_node;
//! \endcond

//! Expression nodes can form a tree to evaluate arbitrary expressions.
//!
//! Expressions are formed from n-ary, user-supplied operators and operands.
//!
//! Operands can be constants, observable values or other expression nodes.
//!
//! The tree will propagate change notifications upwards, so any change to any
//! value<ValueType, EqualityComparator> contained in the tree will be propagated
//! upward, to the root node.
//!
//! When evaluating the root node, only nodes that have been changed will be
//! evaluated.
//!
//! \warning None of the methods in this class can be safely called concurrently.
//! \ingroup observable_detail
template <typename ResultType>
class expression_node final
{
public:
    //! Create a new node from a constant value.
    //!
    //! Nodes created with this constructor are called constant nodes. These nodes
    //! never notify their subscribers of value changes.
    //!
    //! \param[in] constant A constant value that will become the node's evaluated
    //!                     value.
    template <typename ValueType, typename E = std::enable_if_t<
                                                !is_value<ValueType>::value &&
                                                !is_expression_node<ValueType>::value>>
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
    //! Nodes created with this constructor are called value nodes. These nodes
    //! notify their subscribers of value changes as long as the value is alive.
    //!
    //! \param[in] value An observable value who will become the node's evaluated
    //!                  value. This value needs to be kept alive for at least
    //!                  the duration of this constructor call.
    //!
    //! \note If the value is destroyed, the node will keep returning the last
    //!       evaluated value indefinitely.
    //! \note If the value is moved, the node will use the new, moved-into, value.
    template <typename ValueType, typename ... Rest>
    explicit expression_node(value<ValueType, Rest ...> & value)
    {
        static_assert(std::is_convertible<ValueType, ResultType>::value,
                      "ValueType must be convertible to ResultType.");

        auto mark_dirty = [d = data_.get()]() {
                              d->dirty = true;
                              d->notify();
                          };

        auto update_eval = [d = data_.get()](auto & val) {
                                d->eval = [=, v = &val]() {
                                    if(!d->dirty)
                                        return;

                                    d->result = v->get();
                                    d->dirty = false;
                                };
                            };

        data_->subs.emplace_back(value.subscribe(mark_dirty));
        update_eval(value);

        data_->subs.emplace_back(value.moved.subscribe(update_eval));

        data_->subs.emplace_back(
                value.destroyed.subscribe([d = data_.get()]() {
                    d->eval = []() { };
                    d->subs.clear();
                }));

        data_->eval(); // So we cache the value in case the observable value dies
                       // before our node's first eval.
    }

    //! Create a new node from a number of nodes and an n-ary operation.
    //!
    //! Nodes created with this constructor are called n-ary nodes. These nodes
    //! notify their subscribers of changes to the child nodes provided to this
    //! constructor.
    //!
    //! \param[in] op An n-ary operation with the signature below:
    //!
    //!                   ResultType (ValueType const & ...)
    //!
    //! \param[in] nodes ... Expression nodes who's valuewWW will be the operand to the
    //!                      operation.
    template <typename OpType, typename ... ValueType>
    explicit expression_node(OpType && op, expression_node<ValueType> && ... nodes)
    {
        static_assert(std::is_convertible<decltype(op(ValueType { } ...)), ResultType>::value,
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
    void eval() const { data_->eval(); }

    //! Retrieve the expression node's result value.
    //!
    //! This call will not evaluate the node, so this value might be stale. You
    //! can call eval() to make sure that the expression has an updated result
    //! value.
    auto get() const { return data_->result; }

    //! Subscribe to change notifications from this node.
    template <typename Observer>
    auto subscribe(Observer && callable) { return data_->subscribe(callable); }

public:
    //! Expression nodes are not default-constructible.
    expression_node() =default;

    //! Expression nodes are copy-constructible.
    //!
    //! \warning This copy constructor will produce a shallow copy.
    expression_node(expression_node const &) =default;

    //! Expression nodes are copy-assignable.
    //!
    //! \warning This copy assignment operator will produce a shallow copy.
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
        std::enable_if_t<I < std::tuple_size<Tuple>::value>
    {
        std::get<I>(nodes).eval();
        eval_tuple<Tuple, I + 1>(nodes);
    }

    template <typename Tuple, std::size_t I=0>
    static auto eval_tuple(Tuple &) ->
        std::enable_if_t<I >= std::tuple_size<Tuple>::value>
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

//! \cond
template <typename T>
struct is_expression_node_ : std::false_type { };

template <typename T>
struct is_expression_node_<expression_node<T>> : std::true_type { };
//! \endcond

//! Check if a type is an expression node.
//!
//! The static member ``value`` will be true if the provided type is an
//! expression_node.
//!
//! \ingroup observable_detail
template <typename T>
struct is_expression_node : is_expression_node_<std::decay_t<T>> { };

} }

OBSERVABLE_END_CONFIGURE_WARNINGS
