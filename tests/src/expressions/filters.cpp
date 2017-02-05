#include "observable/observe.hpp"
#include "observable/value.hpp"
#include "observable/expressions/operators.hpp"
#include "observable/expressions/filters.hpp"
#include "gtest.h"

namespace {

using observable::value;

auto test_filter_(int a, int b) { return a + b; }
OBSERVABLE_ADAPT_FILTER(test_filter, test_filter_);

TEST(filter_test, adapted_filter_computes_initial_value)
{
    auto val = value<int> { 5 };
    auto res = observe(test_filter(val, 2));

    ASSERT_EQ(5 + 2, res.get());
}

TEST(filter_test, adapted_filter_recomputes_value)
{
    auto val = value<int> { 5 };
    auto res = observe(test_filter(val, 2));

    val = 7;

    ASSERT_EQ(7 + 2, res.get());
}

TEST(filter_test, adapted_filter_can_take_expression_parameters)
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(test_filter(a + b, b + c));

    ASSERT_EQ(1 + 2 + 2 + 3, res.get());

    a = 10;
    b = 20;
    c = 30;

    ASSERT_EQ(10 + 20 + 20 + 30, res.get());
}

TEST(filter_test, adapted_filter_can_participate_in_expression)
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(a + test_filter(b, c) * 20);

    ASSERT_EQ(1 + (2 + 3) * 20, res.get());

    a = 10;
    b = 20;
    c = 30;

    ASSERT_EQ(10 + (20 + 30) * 20, res.get());
}

TEST(filter_test, select)
{
    auto p = value<bool> { true };
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };

    auto res = observe(select(p, a, b));

    ASSERT_EQ(1, res.get());

    a = 10;
    ASSERT_EQ(10, res.get());

    b = 20;
    ASSERT_EQ(10, res.get());

    p = false;
    ASSERT_EQ(20, res.get());

    b = 200;
    ASSERT_EQ(200, res.get());
}

TEST(filter_test, min)
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(min(a, b, c));

    ASSERT_EQ(1, res.get());

    a = 50;
    b = 30;
    c = 40;

    ASSERT_EQ(30, res.get());
}

TEST(filter_test, max)
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(max(a, b, c));

    ASSERT_EQ(3, res.get());

    a = 50;
    b = 30;
    c = 40;

    ASSERT_EQ(50, res.get());
}

TEST(filter_test, mean)
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(mean(a, b, c));

    ASSERT_EQ((1 + 2 + 3) / 3.0, res.get());

    a = 10;
    b = 20;
    c = 30;

    ASSERT_EQ((10 + 20 + 30) / 3.0, res.get());
}

TEST(filter_test, clamp)
{
    auto val = value<int> { 2 };
    auto low = value<int> { 1 };
    auto high = value<int> { 3 };

    auto res = observe(clamp(val, low, high));

    ASSERT_EQ(2, res.get());

    val = 30;
    ASSERT_EQ(3, res.get());

    high = 40;
    ASSERT_EQ(30, res.get());

    low = 35;
    ASSERT_EQ(35, res.get());
}

}
