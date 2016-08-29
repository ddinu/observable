#include <type_traits>
#include <utility>
#include "gtest.h"
#include "observable/detail/handle.hpp"

namespace observable { namespace detail { namespace test {

TEST(handle_test, can_create_handle)
{
    handle { []() {} };
}

TEST(handle_test, unsubscribe_function_is_called)
{
    auto called = false;
    handle h { [&]() { called = true; } };

    h.unsubscribe();

    ASSERT_TRUE(called);
}

TEST(handle_test, destructor_does_not_call_unsubscribe_function)
{
    auto called = false;

    {
        handle h { [&]() { called = true; } };
    }

    ASSERT_FALSE(called);
}

TEST(handle_test, calling_unsubscribe_multiple_times_calls_function_once)
{
    auto call_count = 0;

    handle h { [&]() { ++call_count; } };
    h.unsubscribe();
    h.unsubscribe();

    ASSERT_EQ(call_count, 1);
}

TEST(handle_test, is_copy_constructible)
{
    ASSERT_TRUE(std::is_copy_constructible<handle>::value);
}

TEST(handle_test, is_copy_assignable)
{
    ASSERT_TRUE(std::is_copy_assignable<handle>::value);
}

TEST(handle_test, is_move_constructible)
{
    ASSERT_TRUE(std::is_move_constructible<handle>::value);
}

TEST(handle_test, is_move_assignable)
{
    ASSERT_TRUE(std::is_move_assignable<handle>::value);
}

TEST(handle_test, unsubscribing_from_copy_calls_function)
{
    auto called = false;

    handle h { [&]() { called = true; } };
    auto copy = h;
    copy.unsubscribe();

    ASSERT_TRUE(called);
}

TEST(handle_test, unsubscribing_from_moved_handle_calls_function)
{
    auto called = false;

    handle h { [&]() { called = true; } };
    auto other = std::move(h);
    other.unsubscribe();

    ASSERT_TRUE(called);
}

} } }
