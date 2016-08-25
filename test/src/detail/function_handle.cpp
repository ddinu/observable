#include <memory>
#include "gtest.h"
#include "observable/detail/function_collection.hpp"
#include "observable/detail/function_handle.hpp"

namespace observable { namespace detail { namespace test {

TEST(function_handle_test, can_register_collection)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    function_handle handle { id };
    handle.register_collection(&functions);
}

TEST(function_handle_test, can_call_remove_with_no_collections)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    function_handle handle { id };
    handle.remove();
}

TEST(function_handle_test, can_remove_function_from_collection)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    function_handle handle { id };
    handle.register_collection(&functions);
    handle.remove();

    ASSERT_TRUE(functions.empty());
}

TEST(function_handle_test, can_unregister_collection)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    function_handle handle { id };
    handle.register_collection(&functions);
    handle.unregister_collection(&functions);
    handle.remove();

    ASSERT_FALSE(functions.empty());
}

TEST(function_handle_test, destructor_does_not_remove_function)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    {
        function_handle handle { id };
        handle.register_collection(&functions);
    }

    ASSERT_FALSE(functions.empty());
}

TEST(function_handle_test, can_call_remove_multiple_times)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    function_handle handle { id };
    handle.register_collection(&functions);

    handle.remove();
    handle.remove();
}

TEST(function_handle_test, will_remove_from_multiple_collections)
{
    function_collection functions1;
    auto id = functions1.insert<void()>([]() {});
    auto functions2 = functions1;

    function_handle handle { id };
    handle.register_collection(&functions1);
    handle.register_collection(&functions2);

    handle.remove();

    ASSERT_TRUE(functions1.empty());
    ASSERT_TRUE(functions2.empty());
}

TEST(function_handle_test, additional_removes_do_nothing)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    function_handle handle { id };
    handle.remove();
    handle.register_collection(&functions);
    handle.remove();
}

} } }
