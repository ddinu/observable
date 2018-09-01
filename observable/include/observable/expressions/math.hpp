#pragma once
#include <cmath>
#include <cstdlib>
#include <observable/expressions/filters.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable { inline namespace expr {

//! \cond
namespace filter_detail {

struct abs_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::abs;
        return abs(std::forward<T>(v));
    }
};

struct div_
{
    template <typename X, typename Y>
    auto operator()(X && x, Y && y) const
    {
        using std::div;
        return div(std::forward<X>(x), std::forward<Y>(y));
    }
};

struct exp_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::exp;
        return exp(std::forward<T>(v));
    }
};

struct exp2_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::exp2;
        return exp2(std::forward<T>(v));
    }
};

struct log_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::log;
        return log(std::forward<T>(v));
    }
};

struct log10_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::log10;
        return log10(std::forward<T>(v));
    }
};

struct log2_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::log2;
        return log2(std::forward<T>(v));
    }
};

struct pow_
{
    template <typename Base, typename Exp>
    auto operator()(Base && b, Exp && e) const
    {
        using std::pow;
        return pow(std::forward<Base>(b), std::forward<Exp>(e));
    }
};

struct sqrt_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::sqrt;
        return sqrt(std::forward<T>(v));
    }
};

struct cbrt_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::cbrt;
        return cbrt(std::forward<T>(v));
    }
};

struct hypot_
{
    template <typename X, typename Y>
    auto operator()(X && x, Y && y) const
    {
        using std::hypot;
        return hypot(std::forward<X>(x), std::forward<Y>(y));
    }
};

struct sin_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::sin;
        return sin(std::forward<T>(v));
    }
};

struct cos_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::cos;
        return cos(std::forward<T>(v));
    }
};

struct tan_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::tan;
        return tan(std::forward<T>(v));
    }
};

struct asin_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::asin;
        return asin(std::forward<T>(v));
    }
};

struct acos_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::acos;
        return acos(std::forward<T>(v));
    }
};

struct atan_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::atan;
        return atan(std::forward<T>(v));
    }
};

struct atan2_
{
    template <typename Y, typename X>
    auto operator()(Y && y, X && x) const
    {
        using std::atan2;
        return atan2(std::forward<Y>(y), std::forward<X>(x));
    }
};

struct ceil_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::ceil;
        return ceil(std::forward<T>(v));
    }
};

struct floor_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::floor;
        return floor(std::forward<T>(v));
    }
};

struct trunc_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::trunc;
        return trunc(std::forward<T>(v));
    }
};

struct round_
{
    template <typename T>
    auto operator()(T && v) const
    {
        using std::round;
        return round(std::forward<T>(v));
    }
};

}
//! \endcond

#if defined(DOXYGEN)
//! Compute the absolute value of an integral expression.
//!
//! \see std::abs
//! \ingroup observable_expressions
template <typename T>
auto abs(T && val);
#endif
OBSERVABLE_ADAPT_FILTER(abs, filter_detail::abs_ { })

#if defined(DOXYGEN)
//! Compute both the quotient and the remainder of the division of the
//! numerator x by the denominator y.
//!
//! \see std::div
//! \ingroup observable_expressions
template <typename X, typename Y>
auto div(X && x, Y && y);
#endif
OBSERVABLE_ADAPT_FILTER(div, filter_detail::div_ { })

#if defined(DOXYGEN)
//! Computes e (Euler's number) raised to the given power.
//!
//! \see std::exp
//! \ingroup observable_expressions
template <typename T>
auto exp(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(exp, filter_detail::exp_ { })

#if defined(DOXYGEN)
//! Computes 2 raised to the given power.
//!
//! \see std::exp2
//! \ingroup observable_expressions
template <typename T>
auto exp2(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(exp2, filter_detail::exp2_ { })

#if defined(DOXYGEN)
//! Computes the the natural (base e) logarithm of the provided argument.
//!
//! \see std::log
//! \ingroup observable_expressions
template <typename T>
auto log(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(log, filter_detail::log_ { })

#if defined(DOXYGEN)
//! Computes the common (base-10) logarithm of the provided argument.
//!
//! \see std::log10
//! \ingroup observable_expressions
template <typename T>
auto log10(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(log10, filter_detail::log10_ { })

#if defined(DOXYGEN)
//! Computes the binary (base-2) logarithm of the provided argument.
//!
//! \see std::log2
//! \ingroup observable_expressions
template <typename T>
auto log2(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(log2, filter_detail::log2_ { })

#if defined(DOXYGEN)
//! Computes the value of the provided base raised to the power of the provided
//! exponent.
//!
//! \see std::pow
//! \ingroup observable_expressions
template <typename B, typename E>
auto pow(B && b, E && e);
#endif
OBSERVABLE_ADAPT_FILTER(pow, filter_detail::pow_ { })

#if defined(DOXYGEN)
//! Computes the square root of the provided argument.
//!
//! \see std::sqrt
//! \ingroup observable_expressions
template <typename T>
auto sqrt(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(sqrt, filter_detail::sqrt_ { })

#if defined(DOXYGEN)
//! Computes the cubic root of the provided argument.
//!
//! \see std::cbrt
//! \ingroup observable_expressions
template <typename T>
auto cbrt(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(cbrt, filter_detail::cbrt_ { })

#if defined(DOXYGEN)
//! Computes the square root of the sum of the squares of x and y, without undue
//! overflow or underflow at intermediate stages of the computation.
//!
//! \see std::hypot
//! \ingroup observable_expressions
template <typename X, typename Y>
auto hypot(X && x, Y && y);
#endif
OBSERVABLE_ADAPT_FILTER(hypot, filter_detail::hypot_ { })

#if defined(DOXYGEN)
//! Computes the sine of the provided argument (measured in radians).
//!
//! \see std::sin
//! \ingroup observable_expressions
template <typename T>
auto sin(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(sin, filter_detail::sin_ { })

#if defined(DOXYGEN)
//! Computes the cosine of the provided argument (measured in radians).
//!
//! \see std::cos
//! \ingroup observable_expressions
template <typename T>
auto cos(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(cos, filter_detail::cos_ { })

#if defined(DOXYGEN)
//! Computes the tangent of the provided argument (measured in radians).
//!
//! \see std::tan
//! \ingroup observable_expressions
template <typename T>
auto tan(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(tan, filter_detail::tan_ { })

#if defined(DOXYGEN)
//! Computes the arc sine of the provided argument.
//!
//! \see std::asin
//! \ingroup observable_expressions
template <typename T>
auto asin(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(asin, filter_detail::asin_ { })

#if defined(DOXYGEN)
//! Computes the arc cosine of the provided argument.
//!
//! \see std::acos
//! \ingroup observable_expressions
template <typename T>
auto acos(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(acos, filter_detail::acos_ { })

#if defined(DOXYGEN)
//! Computes the arc tangent of the provided argument.
//!
//! \see std::atan
//! \ingroup observable_expressions
template <typename T>
auto atan(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(atan, filter_detail::atan_ { })

#if defined(DOXYGEN)
//! Computes the arc tangent of y / x using the signs of arguments to determine
//! the correct quadrant.
//!
//! \see std::atan2
//! \ingroup observable_expressions
template <typename Y, typename X>
auto atan2(Y && y, X && x);
#endif
OBSERVABLE_ADAPT_FILTER(atan2, filter_detail::atan2_ { })

#if defined(DOXYGEN)
//! Nearest integer not less than the given value.
//!
//! \see std::ceil
//! \ingroup observable_expressions
template <typename T>
auto ceil(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(ceil, filter_detail::ceil_ { })

#if defined(DOXYGEN)
//! Nearest integer not greater than the given value.
//!
//! \see std::floor
//! \ingroup observable_expressions
template <typename T>
auto floor(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(floor, filter_detail::floor_ { })

#if defined(DOXYGEN)
//! Nearest integer not greater in magnitude than the given value.
//!
//! \see std::trunc
//! \ingroup observable_expressions
template <typename T>
auto trunc(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(trunc, filter_detail::trunc_ { })

#if defined(DOXYGEN)
//! Nearest integer, rounding away from zero in halfway cases.
//!
//! \see std::round
//! \ingroup observable_expressions
template <typename T>
auto round(T && v);
#endif
OBSERVABLE_ADAPT_FILTER(round, filter_detail::round_ { })

} }

OBSERVABLE_END_CONFIGURE_WARNINGS
