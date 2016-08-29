#include <memory>
#include <type_traits>
#include "gtest.h"
#include "observable/detail/handle.hpp"

namespace observable { namespace detail { namespace test {

TEST(auto_handle_test, can_create_auto_remove_handle)
{
    handle h { []() {} };
    auto_handle ah = h;
}

TEST(auto_handle_test, auto_handle_calls_unsubscribe_when_destroyed)
{
    auto called = false;
    handle h { [&]() { called = true; } };

    {
        auto_handle ah = h;
    }

    ASSERT_TRUE(called);
}

TEST(auto_handle_test, can_manually_call_unsubscribe)
{
    auto called = false;

    handle h { [&]() { called = true; } };
    auto_handle ah = h;
    ah.unsubscribe();

    ASSERT_TRUE(called);
}

TEST(auto_handle_test, is_copy_constructible)
{
    ASSERT_TRUE(std::is_copy_constructible<auto_handle>::value);
}

TEST(auto_handle_test, is_copy_assignable)
{
    ASSERT_TRUE(std::is_copy_assignable<auto_handle>::value);
}

TEST(auto_handle_test, is_move_constructible)
{
    ASSERT_TRUE(std::is_move_constructible<auto_handle>::value);
}

TEST(auto_handle_test, is_move_assignable)
{
    ASSERT_TRUE(std::is_move_assignable<auto_handle>::value);
}

TEST(auto_handle_test, unsubscribe_is_called_by_last_instance_destroyed)
{
    auto called = false;

    handle h { [&]() { called = true; } };

    {
        auto_handle ah1 = h;

        {
            auto_handle ah2 = ah1;
        }

        ASSERT_FALSE(called);
    }

    ASSERT_TRUE(called);
}

} } }
