#include <memory>
#include <type_traits>
#include "observable/subscription.hpp"
#include "gtest.h"

namespace observable { namespace test {

TEST(shared_subscription_test, is_default_constructible)
{
    ASSERT_TRUE(std::is_nothrow_default_constructible<shared_subscription>::value);
}

TEST(shared_subscription_test, can_create_shared_subscription)
{
    shared_subscription { unique_subscription { []() {} } };
}

TEST(shared_subscription_test, unsubscribe_is_called_when_destroyed)
{
    auto call_count = 0;

    {
        shared_subscription { unique_subscription { [&]() { ++call_count; } } };
    }

    ASSERT_EQ(call_count, 1);
}

TEST(shared_subscription_test, can_manually_call_unsubscribe)
{
    auto call_count = 0;

    auto sub = shared_subscription { unique_subscription { [&]() { ++call_count; } } };
    sub.unsubscribe();

    ASSERT_EQ(call_count, 1);
}

TEST(shared_subscription_test, is_copy_constructible)
{
    ASSERT_TRUE(std::is_copy_constructible<shared_subscription>::value);
}

TEST(shared_subscription_test, is_copy_assignable)
{
    ASSERT_TRUE(std::is_copy_assignable<shared_subscription>::value);
}

TEST(shared_subscription_test, is_move_constructible)
{
    ASSERT_TRUE(std::is_move_constructible<shared_subscription>::value);
}

TEST(shared_subscription_test, is_move_assignable)
{
    ASSERT_TRUE(std::is_move_assignable<shared_subscription>::value);
}

TEST(shared_subscription_test, unsubscribe_is_called_by_last_instance_destroyed)
{
    auto call_count = 0;

    {
        auto sub = shared_subscription { unique_subscription { [&]() { ++call_count; } } };

        {
            auto copy = sub;
        }

        ASSERT_EQ(call_count, 0);
    }

    ASSERT_EQ(call_count, 1);
}

} }
