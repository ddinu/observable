#pragma once
#include <algorithm>
#include <tuple>
#include <utility>
#include <observable/expressions/utility.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

//! Create an expression filter from a callable.
//!
//! Expression filters take expression nodes as arguments and return an expression
//! node.
//!
//! This macro wraps a regular callable (i.e. one that does not work with
//! expression nodes) and turns it into an expression filter.
//!
//! \param NAME The name that will be used for the expression filter. This must
//!             be a valid function identifier.
//! \param OP Callable instance that will be converted to an expression filter.
//!           This instance must be copy-constructible.
//!
//! \ingroup observable_expressions
#define OBSERVABLE_ADAPT_FILTER(NAME, OP) \
template <typename ... Args> \
inline auto NAME(Args && ... args) \
                -> std::enable_if_t< \
                        ::observable::expr::expr_detail::are_any_observable<Args ...>::value, \
                        ::observable::expr::expr_detail::result_node_t<decltype(OP), Args ...>> \
{ \
    return ::observable::expr::expr_detail::make_node(OP, std::forward<Args>(args) ...); \
}

namespace observable { inline namespace expr {

//! \cond
namespace filter_detail {

template <typename ValueType>
struct construct
{
    template <typename ... T>
    auto operator()(T && ... t) const
    {
        return ValueType { std::forward<T>(t) ... };
    }
};

}
//! \endcond

//! Construct an object.
//!
//! \param args Arguments to be passed to the object's constructor. You must
//!             have at least one observable argument.
//! \return Expression node having the newly constructed object as its result.
//! \tparam ValueType The constructed object's type.
//! \tparam Args Argument pack for the parameters passed to the constructor.
//! \ingroup observable_expressions
template <typename ValueType, typename ... Args>
inline auto construct(Args && ... args)
    -> std::enable_if_t<expr_detail::are_any_observable<Args ...>::value,
                        expression_node<ValueType>>
{
    return expr_detail::make_node(filter_detail::construct<ValueType> { },
                                  std::forward<Args>(args) ...);
}

//! Cast an expression node to another type using static_cast.
//!
//! \param from Incoming type.
//! \return Expression node having the To type.
//!
//! \ingroup observable_expressions
template <typename To, typename From>
inline auto static_expr_cast(From && from)
    -> std::enable_if_t<
    expr_detail::is_observable<From>::value,
    expression_node<To>>
{
    return expr_detail::make_node([](auto && f) { return static_cast<To>(f); },
                                  std::forward<From>(from));
}

//! Cast an expression node to another type using reinterpret_cast.
//!
//! \param from Incoming type.
//! \return Expression node having the To type.
//!
//! \ingroup observable_expressions
template <typename To, typename From>
inline auto reinterpret_expr_cast(From && from)
-> std::enable_if_t<
    expr_detail::is_observable<From>::value,
    expression_node<To>>
{
    return expr_detail::make_node([](auto && f) { return reinterpret_cast<To>(f); },
                                  std::forward<From>(from));
}

//! \cond
namespace filter_detail {
    struct select_
    {
        template <typename CondType,
                  typename TrueType,
                  typename FalseType>
        auto operator()(CondType && cond,
                        TrueType && true_val,
                        FalseType && false_val) const
        {
            return cond ? true_val : false_val;
        }
    };

    struct min_
    {
        template <typename ... Args>
        auto operator()(Args && ... args) const
        {
            using std::min;
            return min({ std::forward<Args>(args) ... });
        }
    };

    struct max_
    {
        template <typename ... Args>
        auto operator()(Args && ... args) const
        {
            using std::max;
            return max({ std::forward<Args>(args) ... });
        }
    };

    struct mean_
    {
        template <typename ... Args>
        auto operator()(Args && ... args) const
        {
            return sum(std::forward<Args>(args) ...) / static_cast<double>(sizeof...(Args));
        }

    private:
        template <typename A, typename ... Rest>
        auto sum(A && a, Rest && ... rest) const
        {
            return a + sum(std::forward<Rest>(rest) ...);
        }

        template <typename A>
        auto sum(A && a) const
        {
            return a;
        }
    };

    struct clamp_
    {
        template <typename Val, typename Low, typename High>
        auto operator()(Val && val, Low && low, High && high) const
        {
            using std::min;
            using std::max;
            return min(high, max(low, val));
        }
    };

    struct zip_
    {
        template <typename ... Args>
        auto operator()(Args && ... args) const
        {
            using std::make_tuple;
            return make_tuple(std::forward<Args>(args) ...);
        }
    };
}
//! \endcond

#if defined(DOXYGEN)
//! Select between two values based on a condition.
//!
//! This is basically the ternary operator for expressions.
//!
//! \param cond A value that evaluates to true or false. This will be used to
//!             choose the return value.
//! \param true_val Value that will be returned if ``cond`` evaluates to true.
//! \param false_val Value that will be returned if ``cond`` evaluates to false.
//! \return One of true_val or false_val.
//!
//! \ingroup observable_expressions
template <typename Cond, typename TrueVal, typename FalseVal>
auto select(Cond && cond, TrueVal && true_val, FalseVal && false_val);
#endif
OBSERVABLE_ADAPT_FILTER(select, filter_detail::select_ { })

#if defined(DOXYGEN)
//! Return the argument with the minimum value.
//!
//! Will use the less-than operator to compare values.
//!
//! \param values ... Arguments from which the minimum will be chosen.
//! \return The minimum argument.
//!
//! \ingroup observable_expressions
template <typename ... Values>
auto min(Values ... values);
#endif
OBSERVABLE_ADAPT_FILTER(min, filter_detail::min_ { })

#if defined(DOXYGEN)
//! Return the argument with the maximum value.
//!
//! Will use the less-than operator to compare values.
//!
//! \param values ... Arguments from which the maximum will be chosen.
//! \return The maximum argument.
//!
//! \ingroup observable_expressions
template <typename ... Values>
auto max(Values ... values);
#endif
OBSERVABLE_ADAPT_FILTER(max, filter_detail::max_ { })

#if defined(DOXYGEN)
//! Return the mean of the arguments.
//!
//! \param values ... Arguments for which the mean will be computed.
//! \return The mean value of the arguments.
//!
//! \ingroup observable_expressions
template <typename ... Values>
auto mean(Values ... values);
#endif
OBSERVABLE_ADAPT_FILTER(mean, filter_detail::mean_ { })

#if defined(DOXYGEN)
//! Keep a value between a minimum and maximum.
//!
//! Will use the less-than operator to compare values.
//!
//! \param val Value to clamp.
//! \param low The minimum allowed value of the ``val`` parameter.
//! \param high The maximum allowed value of the ``val`` parameter.
//! \return The clamped value.
//!
//! \ingroup observable_expressions
template <typename Val, typename Low, typename High>
auto clamp(Val && val, Low && low, High && high);
#endif
OBSERVABLE_ADAPT_FILTER(clamp, filter_detail::clamp_ { })

#if defined(DOXYGEN)
//! Convert a number of arguments to a tuple containing the arguments.
//!
//! \param args ... Arguments to pack into a tuple.
//! \return Tuple containing the provided arguments.
//!
//! \ingroup observable_expressions
template <typename ... Args>
auto zip(Args && ... args);
#endif
OBSERVABLE_ADAPT_FILTER(zip, filter_detail::zip_ { })

} }

OBSERVABLE_END_CONFIGURE_WARNINGS
