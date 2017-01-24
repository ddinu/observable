#include <type_traits>
#include <utility>
#include "observable/subscription.hpp"
#include "gtest.h"

namespace observable { namespace test {

TEST(unique_subscription_test, is_default_constructible)
{
    ASSERT_TRUE(std::is_default_constructible<unique_subscription>::value);
}

TEST(unique_subscription_test, can_create_initialized_subscription)
{
    unique_subscription { []() {} };
}

TEST(unique_subscription_test, unsubscribe_function_is_called)
{
    auto call_count = 0;
    auto sub = unique_subscription { [&]() { ++call_count; } };

    sub.unsubscribe();

    ASSERT_EQ(call_count, 1);
}

TEST(unique_subscription_test, destructor_calls_unsubscribe_function)
{
    auto call_count = 0;

    {
        unique_subscription { [&]() { ++call_count; } };
    }

    ASSERT_EQ(call_count, 1);
}

TEST(unique_subscription_test, calling_unsubscribe_multiple_times_calls_function_once)
{
    auto call_count = 0;

    auto sub = unique_subscription { [&]() { ++call_count; } };
    sub.unsubscribe();
    sub.unsubscribe();

    ASSERT_EQ(call_count, 1);
}

TEST(unique_subscription_test, is_not_copy_constructible)
{
    ASSERT_FALSE(std::is_copy_constructible<unique_subscription>::value);
}

TEST(unique_subscription_test, is_not_copy_assignable)
{
    ASSERT_FALSE(std::is_copy_assignable<unique_subscription>::value);
}

TEST(unique_subscription_test, is_move_constructible)
{
    ASSERT_TRUE(std::is_move_constructible<unique_subscription>::value);
}

TEST(unique_subscription_test, is_move_assignable)
{
    ASSERT_TRUE(std::is_move_assignable<unique_subscription>::value);
}

TEST(unique_subscription_test, unsubscribing_from_moved_handle_calls_function)
{
    auto call_count = 0;

    {
        auto sub = unique_subscription { [&]() { ++call_count; } };
        auto other = std::move(sub);
        other.unsubscribe();
    }

    ASSERT_EQ(call_count, 1);
}

} }
