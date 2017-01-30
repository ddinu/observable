#include <type_traits>
#include <utility>
#include "observable/subscription.hpp"
#include "gtest.h"

namespace observable { namespace test {

TEST(infinite_subscription_test, is_default_constructible)
{
    ASSERT_TRUE(std::is_default_constructible<infinite_subscription>::value);
}

TEST(infinite_subscription_test, can_create_initialized_subscription)
{
    infinite_subscription { []() {} };
}

TEST(infinite_subscription_test, unsubscribe_function_is_called)
{
    auto call_count = 0;
    auto sub = infinite_subscription { [&]() { ++call_count; } };

    sub.unsubscribe();

    ASSERT_EQ(call_count, 1);
}

TEST(infinite_subscription_test, destructor_does_not_call_unsubscribe_function)
{
    auto call_count = 0;

    {
        infinite_subscription { [&]() { ++call_count; } };
    }

    ASSERT_EQ(call_count, 0);
}

TEST(infinite_subscription_test, calling_unsubscribe_multiple_times_calls_function_once)
{
    auto call_count = 0;

    auto sub = infinite_subscription { [&]() { ++call_count; } };
    sub.unsubscribe();
    sub.unsubscribe();

    ASSERT_EQ(call_count, 1);
}

TEST(infinite_subscription_test, is_not_copy_constructible)
{
    ASSERT_FALSE(std::is_copy_constructible<infinite_subscription>::value);
}

TEST(infinite_subscription_test, is_not_copy_assignable)
{
    ASSERT_FALSE(std::is_copy_assignable<infinite_subscription>::value);
}

TEST(infinite_subscription_test, is_move_constructible)
{
    ASSERT_TRUE(std::is_move_constructible<infinite_subscription>::value);
}

TEST(infinite_subscription_test, is_move_assignable)
{
    ASSERT_TRUE(std::is_move_assignable<infinite_subscription>::value);
}

TEST(infinite_subscription_test, unsubscribing_from_moved_subscription_calls_function)
{
    auto call_count = 0;
    infinite_subscription other;

    {
        auto sub = infinite_subscription { [&]() { ++call_count; } };
        other = std::move(sub);
    }

    other.unsubscribe();

    ASSERT_EQ(call_count, 1);
}

} }
