#include <cmath>
#include "observable/observe.hpp"
#include "observable/value.hpp"
#include "observable/expressions/math.hpp"
#include "gtest.h"

namespace {

using observable::value;

TEST(math_filter_test, abs)
{
    auto a = value<int> { 5 };
    auto res = observe(abs(a));

    ASSERT_EQ(5, res.get());

    a = -7;
    ASSERT_EQ(7, res.get());
}

TEST(math_filter_test, div)
{
    auto x = value<int> { 5 };
    auto y = value<int> { 3 };
    auto res = observe(div(x, y));

    ASSERT_EQ(std::div(5, 3).quot, res.get().quot);
    ASSERT_EQ(std::div(5, 3).rem, res.get().rem);

    x = 31;
    ASSERT_EQ(std::div(31, 3).quot, res.get().quot);
    ASSERT_EQ(std::div(31, 3).rem, res.get().rem);
}

TEST(math_filter_test, exp)
{
    auto a = value<double> { 5 };
    auto res = observe(exp(a));

    ASSERT_DOUBLE_EQ(std::exp(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::exp(7.0), res.get());
}

TEST(math_filter_test, exp2)
{
    auto a = value<double> { 5 };
    auto res = observe(exp2(a));

    ASSERT_DOUBLE_EQ(std::exp2(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::exp2(7.0), res.get());
}

TEST(math_filter_test, log)
{
    auto a = value<double> { 5 };
    auto res = observe(log(a));

    ASSERT_DOUBLE_EQ(std::log(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::log(7.0), res.get());
}

TEST(math_filter_test, log10)
{
    auto a = value<double> { 5 };
    auto res = observe(log10(a));

    ASSERT_DOUBLE_EQ(std::log10(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::log10(7.0), res.get());
}

TEST(math_filter_test, log2)
{
    auto a = value<double> { 5 };
    auto res = observe(log2(a));

    ASSERT_DOUBLE_EQ(std::log2(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::log2(7.0), res.get());
}

TEST(math_filter_test, pow)
{
    auto b = value<double> { 5 };
    auto e = value<double> { 3 };
    auto res = observe(pow(b, e));

    ASSERT_DOUBLE_EQ(std::pow(5, 3), res.get());

    b = 7;
    ASSERT_DOUBLE_EQ(std::pow(7, 3), res.get());

    e = 4;
    ASSERT_DOUBLE_EQ(std::pow(7, 4), res.get());
}

TEST(math_filter_test, sqrt)
{
    auto a = value<double> { 5 };
    auto res = observe(sqrt(a));

    ASSERT_DOUBLE_EQ(std::sqrt(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::sqrt(7.0), res.get());
}

TEST(math_filter_test, cbrt)
{
    auto a = value<double> { 5 };
    auto res = observe(cbrt(a));

    ASSERT_DOUBLE_EQ(std::cbrt(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::cbrt(7.0), res.get());
}

TEST(math_filter_test, hypot)
{
    auto x = value<double> { 5 };
    auto y = value<double> { 7 };
    auto res = observe(hypot(x, y));

    ASSERT_DOUBLE_EQ(std::hypot(5, 7), res.get());

    x = 10;
    ASSERT_DOUBLE_EQ(std::hypot(10, 7), res.get());

    y = 20;
    ASSERT_DOUBLE_EQ(std::hypot(10, 20), res.get());
}

TEST(math_filter_test, sin)
{
    auto a = value<double> { 5 };
    auto res = observe(sin(a));

    ASSERT_DOUBLE_EQ(std::sin(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::sin(7.0), res.get());
}

TEST(math_filter_test, cos)
{
    auto a = value<double> { 5 };
    auto res = observe(cos(a));

    ASSERT_DOUBLE_EQ(std::cos(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::cos(7.0), res.get());
}

TEST(math_filter_test, tan)
{
    auto a = value<double> { 5 };
    auto res = observe(tan(a));

    ASSERT_DOUBLE_EQ(std::tan(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::tan(7.0), res.get());
}

TEST(math_filter_test, asin)
{
    auto a = value<double> { 0.5 };
    auto res = observe(asin(a));

    ASSERT_DOUBLE_EQ(std::asin(0.5), res.get());

    a = 0.8;
    ASSERT_DOUBLE_EQ(std::asin(0.8), res.get());
}

TEST(math_filter_test, acos)
{
    auto a = value<double> { 0.5 };
    auto res = observe(acos(a));

    ASSERT_DOUBLE_EQ(std::acos(0.5), res.get());

    a = 0.8;
    ASSERT_DOUBLE_EQ(std::acos(0.8), res.get());
}

TEST(math_filter_test, atan)
{
    auto a = value<double> { 5 };
    auto res = observe(atan(a));

    ASSERT_DOUBLE_EQ(std::atan(5.0), res.get());

    a = 7;
    ASSERT_DOUBLE_EQ(std::atan(7.0), res.get());
}

TEST(math_filter_test, atan2)
{
    auto y = value<double> { 5 };
    auto x = value<double> { 7 };
    auto res = observe(atan2(y, x));

    ASSERT_DOUBLE_EQ(std::atan2(5, 7), res.get());

    y = 17;
    ASSERT_DOUBLE_EQ(std::atan2(17, 7), res.get());

    x = 10;
    ASSERT_DOUBLE_EQ(std::atan2(17, 10), res.get());
}

TEST(math_filter_test, ceil)
{
    auto a = value<double> { 0.4 };
    auto res = observe(ceil(a));

    ASSERT_DOUBLE_EQ(1.0, res.get());

    a = 1.8;
    ASSERT_DOUBLE_EQ(2, res.get());
}

TEST(math_filter_test, floor)
{
    auto a = value<double> { 0.7 };
    auto res = observe(floor(a));

    ASSERT_DOUBLE_EQ(0, res.get());

    a = 1.8;
    ASSERT_DOUBLE_EQ(1, res.get());
}

TEST(math_filter_test, trunc)
{
    auto a = value<double> { 0.7 };
    auto res = observe(trunc(a));

    ASSERT_DOUBLE_EQ(0, res.get());

    a = 1.8;
    ASSERT_DOUBLE_EQ(1, res.get());
}

TEST(math_filter_test, round)
{
    auto a = value<double> { 0.4 };
    auto res = observe(round(a));

    ASSERT_DOUBLE_EQ(0, res.get());

    a = 1.8;
    ASSERT_DOUBLE_EQ(2, res.get());
}

}
