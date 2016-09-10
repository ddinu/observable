#include <utility>
#include "gtest.h"
#include "observable/detail/function_wrapper.hpp"

namespace observable { namespace detail { namespace test {

TEST(function_wrapper_test, can_create_wrapper)
{
    function_wrapper wrapper { std::function<void()> { }, 0, 0 };
}

TEST(function_wrapper_test, can_call_wrapped_function_with_no_arguments)
{
    auto call_count = 0;
    function_wrapper wrapper {
        std::function<void()> { [&]() { ++call_count; } },
        0, 0
    };

    wrapper.function<void()>()();

    ASSERT_EQ(call_count, 1);
}

TEST(function_wrapper_test, can_call_wrapped_function_with_multiple_arguments)
{
    auto sum = 0;
    function_wrapper wrapper {
        std::function<void(int, int)> { [&](int a, int b) { sum += a + b; } },
        0, 0
    };

    wrapper.function<void(int, int)>()(3, 4);

    ASSERT_EQ(sum, 3 + 4);
}

TEST(function_wrapper_test, can_copy_wrapper)
{
    auto call_count = 0;

    function_wrapper w2;

    {
        function_wrapper w1 {
            std::function<void()> { [&]() {++call_count; } },
            0, 0
        };

        w2 = w1;

        w1.function<void()>()();
    }

    w2.function<void()>()();

    ASSERT_EQ(call_count, 2);
}

TEST(function_wrapper_test, can_move_wrapper)
{
    auto call_count = 0;

    function_wrapper w2;

    {
        function_wrapper w1 {
            std::function<void()> { [&]() { ++call_count;  } },
            0, 0
        };

        w2 = std::move(w1);
    }

    w2.function<void()>()();

    ASSERT_EQ(call_count, 1);
}

} } }
