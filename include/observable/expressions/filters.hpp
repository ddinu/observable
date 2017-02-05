#pragma once
#include <algorithm>
#include <utility>
#include "observable/expressions/utility.hpp"

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
auto NAME(Args && ... args) \
    -> std::enable_if_t< \
            ::observable::expr::expr_detail::are_any_observable<Args ...>::value, \
            ::observable::expr::expr_detail::result_node_t<decltype(OP), Args ...>> \
{ \
    return ::observable::expr::expr_detail::make_node(OP, std::forward<Args>(args) ...); \
}

namespace observable { inline namespace expr {

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
#endif

} }
