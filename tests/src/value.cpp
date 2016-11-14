#include <type_traits>
#include "gtest.h"
#include "observable/value.hpp"

namespace observable { namespace test {

TEST(value_test, can_create_value)
{
    value<int> { };
}

TEST(value_test, can_create_initialized_value)
{
    value<int> { 123 };
}

TEST(value_test, can_get_value)
{
    value<int> val { 123 };

    ASSERT_EQ(val.get(), 123);
}

TEST(value_test, can_convert_to_value_type)
{
    value<int> val { 123 };
    int v = val;

    ASSERT_EQ(v, 123);
}

TEST(value_test, can_subscribe_to_value_changes)
{
    auto call_count = 0;

    value<int> val { 123 };
    val.subscribe([&]() { ++call_count; }).release();
    val.set(1234);

    ASSERT_EQ(call_count, 1);
}

TEST(value_test, can_subscribe_to_value_changes_and_get_value)
{
    auto call_count = 0;
    auto v = 0;

    value<int> val { 123 };
    val.subscribe([&](int new_v) { ++call_count; v = new_v; }).release();
    val.set(1234);

    ASSERT_EQ(call_count, 1);
    ASSERT_EQ(v, 1234);
}

TEST(value_test, setting_same_value_does_not_trigger_subscribers)
{
    auto call_count = 0;

    value<int> val { 123 };
    val.subscribe([&]() { ++call_count; });
    val.subscribe([&](int) { ++call_count; });
    val.set(123);

    ASSERT_EQ(call_count, 0);
}

TEST(value_test, copy_constructed_value_is_correct)
{
    value<int> val { 123 };
    value<int> val_copy { val };

    ASSERT_EQ(val_copy.get(), 123);
}

TEST(value_test, copy_assigned_value_is_correct)
{
    value<int> val { 123 };
    value<int> val_copy = val;

    ASSERT_EQ(val_copy.get(), 123);
}

TEST(value_test, copy_constructed_value_has_no_subscribers)
{
    auto call_count = 0;

    value<int> val { 123 };
    val.subscribe([&]() { ++call_count; }).release();

    value<int> val_copy { val };
    val_copy.set(1234);

    ASSERT_EQ(call_count, 0);
}

TEST(value_test, copy_assigned_value_has_no_subscribers)
{
    auto call_count = 0;

    value<int> val { 123 };
    val.subscribe([&]() { ++call_count; }).release();

    value<int> val_copy = val;
    val_copy.set(1234);

    ASSERT_EQ(call_count, 0);
}

TEST(value_test, move_constructed_value_is_correct)
{
    value<std::unique_ptr<int>> val { std::make_unique<int>(123) };
    value<std::unique_ptr<int>> moved_val { std::move(val) };

    ASSERT_EQ(*moved_val.get(), 123);
}

TEST(value_test, move_assigned_value_is_correct)
{
    value<std::unique_ptr<int>> val { std::make_unique<int>(123) };
    value<std::unique_ptr<int>> moved_val = std::move(val);

    ASSERT_EQ(*moved_val.get(), 123);
}

TEST(value_test, move_constructed_value_keeps_subscribers)
{
    auto call_count = 0;

    value<std::unique_ptr<int>> val { std::make_unique<int>(123) };
    val.subscribe([&]() { ++call_count; }).release();

    value<std::unique_ptr<int>> moved_val { std::move(val) };
    moved_val.set(std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

TEST(value_test, move_assigned_value_keeps_subscribers)
{
    auto call_count = 0;

    value<std::unique_ptr<int>> val { std::make_unique<int>(123) };
    val.subscribe([&]() { ++call_count; }).release();

    value<std::unique_ptr<int>> moved_val = std::move(val);
    moved_val.set(std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

} }
