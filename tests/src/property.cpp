#include <type_traits>
#include "gtest.h"
#include "observable/property.hpp"

namespace observable { namespace test {

struct property_test : testing::Test
{
    template <typename P, typename V>
    void set(P & prop, V v) { prop.set(std::move(v)); }

    virtual ~property_test();
};

property_test::~property_test() { } // To shut up clang.

TEST_F(property_test, can_create_property)
{
    property<int> { };
}

TEST_F(property_test, can_create_initialized_property)
{
    property<int> { 123 };
}

TEST_F(property_test, can_get_property_value)
{
    property<int> prop { 123 };

    ASSERT_EQ(prop.get(), 123);
}

TEST_F(property_test, can_convert_to_value_type)
{
    property<int> prop { 123 };
    int val = prop;

    ASSERT_EQ(val, 123);
}

TEST_F(property_test, can_subscribe_to_value_changes)
{
    auto call_count = 0;

    property<int, property_test> prop { 123 };
    prop.subscribe([&]() { ++call_count; }).release();
    set(prop, 1234);

    ASSERT_EQ(call_count, 1);
}

TEST_F(property_test, can_subscribe_to_value_changes_and_get_value)
{
    auto call_count = 0;
    auto value = 0;

    property<int, property_test> prop { 123 };
    prop.subscribe([&](int v) { ++call_count; value = v; }).release();
    set(prop, 1234);

    ASSERT_EQ(call_count, 1);
    ASSERT_EQ(value, 1234);
}

TEST_F(property_test, setting_same_value_does_not_trigger_subscribers)
{
    auto call_count = 0;

    property<int, property_test> prop { 123 };
    prop.subscribe([&]() { ++call_count; });
    prop.subscribe([&](int) { ++call_count; });
    set(prop, 123);

    ASSERT_EQ(call_count, 0);
}

TEST_F(property_test, copy_constructed_property_has_correct_value)
{
    property<int> prop { 123 };
    property<int> prop_copy { prop };

    ASSERT_EQ(prop_copy.get(), 123);
}

TEST_F(property_test, copy_assigned_property_has_correct_value)
{
    property<int> prop { 123 };
    property<int> prop_copy = prop;

    ASSERT_EQ(prop_copy.get(), 123);
}

TEST_F(property_test, copy_constructed_property_has_no_subscribers)
{
    auto call_count = 0;

    property<int, property_test> prop { 123 };
    prop.subscribe([&]() { ++call_count; }).release();

    property<int, property_test> prop_copy { prop };
    set(prop_copy, 1234);

    ASSERT_EQ(call_count, 0);
}

TEST_F(property_test, copy_assigned_property_has_no_subscribers)
{
    auto call_count = 0;

    property<int, property_test> prop { 123 };
    prop.subscribe([&]() { ++call_count; }).release();

    property<int, property_test> prop_copy = prop;
    set(prop_copy, 1234);

    ASSERT_EQ(call_count, 0);
}

TEST_F(property_test, move_constructed_property_has_correct_value)
{
    property<std::unique_ptr<int>> p { std::make_unique<int>(123) };
    property<std::unique_ptr<int>> moved_prop { std::move(p) };

    ASSERT_EQ(*moved_prop.get(), 123);
}

TEST_F(property_test, move_assigned_property_has_correct_value)
{
    property<std::unique_ptr<int>> prop { std::make_unique<int>(123) };
    property<std::unique_ptr<int>> moved_prop = std::move(prop);

    ASSERT_EQ(*moved_prop.get(), 123);
}

TEST_F(property_test, move_constructed_property_keeps_subscribers)
{
    auto call_count = 0;

    property<std::unique_ptr<int>, property_test> prop { std::make_unique<int>(123) };
    prop.subscribe([&]() { ++call_count; }).release();

    property<std::unique_ptr<int>, property_test> moved_prop  { std::move(prop) };
    set(moved_prop, std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

TEST_F(property_test, move_assigned_property_keeps_subscribers)
{
    auto call_count = 0;

    property<std::unique_ptr<int>, property_test> prop { std::make_unique<int>(123) };
    prop.subscribe([&]() { ++call_count; }).release();

    property<std::unique_ptr<int>, property_test> moved_prop = std::move(prop);
    set(moved_prop, std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

} }
