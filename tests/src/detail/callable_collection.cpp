#include <type_traits>
#include <memory>
#include "gtest.h"
#include "observable/detail/callable_collection.hpp"

namespace observable { namespace detail { namespace test {

TEST(callable_collection_test, default_constructed_collection_is_empty)
{
    callable_collection<void()> collection;
    ASSERT_TRUE(collection.empty());
}

TEST(callable_collection_test, default_constructed_collection_has_size_zero)
{
    callable_collection<void()> collection;
    ASSERT_EQ(collection.size(), 0u);
}

TEST(callable_collection_test, can_insert)
{
    callable_collection<void()> collection;
    auto id = collection.insert([]() {});

    ASSERT_TRUE(!!id);
}

TEST(callable_collection_test, different_ids_compare_different)
{
    callable_collection<void()> collection;
    auto id1 = collection.insert([]() {});
    auto id2 = collection.insert([]() {});

    ASSERT_NE(id1, id2);
}

TEST(callable_collection_test, collection_is_not_empty_after_insert)
{
    callable_collection<void()> collection;
    collection.insert([]() {});

    ASSERT_FALSE(collection.empty());
}

TEST(callable_collection_test, size_increases_after_insert)
{
    callable_collection<void()> collection;
    collection.insert([]() {});

    ASSERT_GT(collection.size(), 0u);
}

TEST(callable_collection_test, size_is_correct)
{
    callable_collection<void()> collection;

    collection.insert([]() {});
    collection.insert([]() { });

    ASSERT_EQ(collection.size(), 2u);
}

TEST(callable_collection_test, can_remove)
{
    callable_collection<void()> collection;

    auto id = collection.insert([]() {});
    collection.remove(id);

    ASSERT_TRUE(collection.empty());
}

TEST(callable_collection_test, functions_are_called)
{
    callable_collection<void()> collection;
    auto call_count = 0;

    collection.insert([&]() { ++call_count; });
    collection.insert([&]() { ++call_count; });
    collection.call_all();

    ASSERT_EQ(call_count, 2);
}

TEST(callable_collection_test, call_all_does_nothing_for_empty_collection)
{
    callable_collection<void()> collection;

    collection.call_all();
}

TEST(callable_collection_test, call_arguments_are_passed_to_functions)
{
    callable_collection<void(int)> collection;
    auto result = 0;

    collection.insert([&](auto v) { result += v; });
    collection.insert([&](auto v) { result += v; });

    collection.call_all(3);

    ASSERT_EQ(result, 6);
}

TEST(callable_collection_test, reference_call_arguments_are_not_copied)
{
    callable_collection<void(std::unique_ptr<int> &)> collection;
    auto call_count = std::make_unique<int>(0);

    collection.insert([](auto && c) { ++(*c); });
    collection.call_all(call_count);

    ASSERT_EQ(*call_count, 1);
}

TEST(callable_collection_test, is_copy_constructible)
{
    ASSERT_TRUE(std::is_copy_constructible<callable_collection<void()>>::value);
}

TEST(callable_collection_test, is_copy_assignable)
{
    ASSERT_TRUE(std::is_copy_assignable<callable_collection<void()>>::value);
}

TEST(callable_collection_test, is_move_constructible)
{
    ASSERT_TRUE(std::is_move_constructible<callable_collection<void()>>::value);
}

TEST(callable_collection_test, is_move_assignable)
{
    ASSERT_TRUE(std::is_move_assignable<callable_collection<void()>>::value);
}

TEST(callable_collection_test, content_is_copied)
{
    callable_collection<void()> collection;

    auto call_count = 0;
    collection.insert([&]() { ++call_count; });

    auto copy = collection;
    copy.call_all();

    ASSERT_EQ(call_count, 1);
}

TEST(callable_collection_test, content_is_moved)
{
    callable_collection<void()> collection;

    auto call_count = 0;
    collection.insert([&]() { ++call_count; });

    auto other = std::move(collection);
    other.call_all();

    ASSERT_EQ(call_count, 1);
}

TEST(callable_collection_test, id_is_contained_in_copied_collection)
{
    callable_collection<void()> collection;
    auto id = collection.insert([]() { });

    auto copy = collection;

    ASSERT_TRUE(copy.remove(id));
}

TEST(callable_collection_test, object_is_not_removed_from_copied_collection)
{
    callable_collection<void()> collection;
    auto id = collection.insert([]() { });

    auto copy = collection;
    collection.remove(id);

    ASSERT_FALSE(copy.empty());
}

} } }
