#include <gtest/gtest.h>
#include "observable/detail/function_collection.hpp"

namespace observable { namespace detail { namespace test {

TEST(function_collection_test, default_constructed_collection_is_empty)
{
    function_collection functions;
    ASSERT_TRUE(functions.empty());
}

TEST(function_collection_test, default_constructed_collection_has_size_zero)
{
    function_collection functions;
    ASSERT_EQ(functions.size(), 0);
}

TEST(function_collection_test, can_insert_function)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    ASSERT_TRUE(!!id);
}

TEST(function_collection_test, different_ids_compare_different)
{
    function_collection functions;
    auto id1 = functions.insert<void()>([]() {});
    auto id2 = functions.insert<void()>([]() {});

    ASSERT_NE(id1, id2);
}

TEST(function_collection_test, collection_is_not_empty_after_insert)
{
    function_collection functions;
    functions.insert<void()>([]() {});

    ASSERT_FALSE(functions.empty());
}

TEST(function_collection_test, size_increases_after_insert)
{
    function_collection functions;
    functions.insert<void()>([]() {});

    ASSERT_EQ(functions.size(), 1);
}

TEST(function_collection_test, size_is_correct)
{
    function_collection functions;

    functions.insert<void()>([]() {});
    functions.insert<void(float)>([](auto) { });

    ASSERT_EQ(functions.size(), 2);
}

TEST(function_collection_test, can_remove_function)
{
    function_collection functions;

    auto id = functions.insert<void()>([]() {});
    functions.remove(id);

    ASSERT_TRUE(functions.empty());
}

TEST(function_collection_test, functions_are_called)
{
    function_collection functions;
    auto call_count = 0;

    functions.insert<void()>([&]() { ++call_count; });
    functions.insert<void()>([&]() { ++call_count; });
    functions.call_all<void()>();

    ASSERT_EQ(call_count, 2);
}

TEST(function_collection_test, call_all_returns_number_of_called_functions)
{
    function_collection functions;

    functions.insert<void()>([]() { });
    functions.insert<void()>([]() { });
    auto call_count = functions.call_all<void()>();

    ASSERT_EQ(call_count, 2);
}

TEST(function_collection_test, call_all_returns_zero_for_empty_collection)
{
    function_collection functions;

    auto call_count = functions.call_all<void()>();

    ASSERT_EQ(call_count, 0);
}

TEST(function_collection_test, call_arguments_are_passed_to_functions)
{
    function_collection functions;
    auto result = 0;

    functions.insert<void(int)>([&](auto v) { result += v; });
    functions.insert<void(int)>([&](auto v) { result += v; });

    functions.call_all<void(int)>(3);

    ASSERT_EQ(result, 6);
}

TEST(function_collection_test, reference_call_arguments_are_not_copied)
{
    function_collection functions;
    auto call_count = std::make_unique<int>(0);

    functions.insert<void(std::unique_ptr<int> &)>([](auto && c) { ++(*c); });
    functions.call_all<void(std::unique_ptr<int> &)>(call_count);

    ASSERT_EQ(*call_count, 1);
}

TEST(function_collection_test, only_matching_functions_are_called)
{
    function_collection functions;
    auto call_count = 0;

    functions.insert<void()>([&]() { ++call_count; });
    functions.insert<void(int)>([&](auto) { ++call_count; });
    functions.insert<void(double)>([&](auto) { ++call_count; });
    functions.insert<void(float)>([&](auto) { ++call_count; });

    functions.call_all<void(float)>(5.0f);

    ASSERT_EQ(call_count, 1);
}

TEST(function_collection_test, call_all_returns_zero_if_no_function_matches)
{
    function_collection functions;

    functions.insert<void(int)>([](auto) {});
    auto call_count = functions.call_all<void()>();

    ASSERT_EQ(call_count, 0);
}

TEST(function_collection_test, function_return_values_are_ignored)
{
    function_collection functions;

    functions.insert<int()>([]() { return 5; });
    auto call_count = functions.call_all<int()>();

    ASSERT_EQ(call_count, 1);
}

TEST(function_collection_test, functions_are_copied)
{
    function_collection functions;

    auto call_count = 0;
    auto id = functions.insert<void()>([&]() { ++call_count; });

    auto copy = functions;
    copy.call_all<void()>();

    ASSERT_EQ(call_count, 1);
}

TEST(function_collection_test, function_ids_are_valid_in_collection_copies)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});
    auto copy = functions;

    copy.remove(id);

    ASSERT_TRUE(copy.empty());
}

TEST(function_collection_test, function_is_only_removed_from_copy)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});
    auto copy = functions;

    copy.remove(id);

    ASSERT_FALSE(functions.empty());
}

TEST(function_collection_test, contains_returns_true_for_contained_id)
{
    function_collection functions;
    auto id = functions.insert<void()>([]() {});

    ASSERT_TRUE(functions.contains(id));
}

TEST(function_collection_test, contains_returns_false_for_uncontained_id)
{
    function_collection functions1;
    function_collection functions2;
    auto id = functions2.insert<void()>([]() {});

    ASSERT_FALSE(functions1.contains(id));
}

} } }
