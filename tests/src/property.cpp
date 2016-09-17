#include <type_traits>
#include "gtest.h"
#include "observable/property.hpp"

namespace observable { namespace test {

struct nomove_nocopy
{
    nomove_nocopy() =default;
    nomove_nocopy(nomove_nocopy const &) =delete;
    nomove_nocopy(nomove_nocopy &&) =delete;
    nomove_nocopy & operator=(nomove_nocopy const &) =delete;
    nomove_nocopy & operator=(nomove_nocopy &&) =delete;
};

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
    property<int> p { 123 };

    ASSERT_EQ(p.value(), 123);
}

TEST_F(property_test, can_convert_to_value_type)
{
    property<int> p { 123 };
    int v = p;

    ASSERT_EQ(v, 123);
}

TEST_F(property_test, can_subscribe_to_value_changes)
{
    auto call_count = 0;

    property<int, property_test> p { 123 };
    p.subscribe([&]() { ++call_count; }).release();
    set(p, 1234);

    ASSERT_EQ(call_count, 1);
}

TEST_F(property_test, can_subscribe_to_value_changes_and_get_value)
{
    auto call_count = 0;
    auto value = 0;

    property<int, property_test> p { 123 };
    p.subscribe([&](int v) { ++call_count; value = v; }).release();
    set(p, 1234);

    ASSERT_EQ(call_count, 1);
    ASSERT_EQ(value, 1234);
}

TEST_F(property_test, setting_same_value_does_not_trigger_subscribers)
{
    auto call_count = 0;

    property<int, property_test> p { 123 };
    p.subscribe([&]() { ++call_count; });
    p.subscribe([&](int) { ++call_count; });
    set(p, 123);

    ASSERT_EQ(call_count, 0);
}

TEST_F(property_test, copy_constructed_property_has_correct_value)
{
    property<int> p { 123 };
    auto c { p };

    ASSERT_EQ(c.value(), 123);
}

TEST_F(property_test, copy_assigned_property_has_correct_value)
{
    property<int> p { 123 };
    property<int> c = p;

    ASSERT_EQ(c.value(), 123);
}

TEST_F(property_test, copy_constructed_property_has_no_subscribers)
{
    auto call_count = 0;

    property<int, property_test> p { 123 };
    p.subscribe([&]() { ++call_count; }).release();

    auto c { p };
    set(c, 1234);

    ASSERT_EQ(call_count, 0);
}

TEST_F(property_test, copy_assigned_property_has_no_subscribers)
{
    auto call_count = 0;

    property<int, property_test> p { 123 };
    p.subscribe([&]() { ++call_count; }).release();

    auto c = p;
    set(c, 1234);

    ASSERT_EQ(call_count, 0);
}

TEST_F(property_test, move_constructed_property_has_correct_value)
{
    property<std::unique_ptr<int>> p { std::make_unique<int>(123) };
    auto c { std::move(p) };

    ASSERT_EQ(*c.value(), 123);
}

TEST_F(property_test, move_assigned_property_has_correct_value)
{
    property<std::unique_ptr<int>> p { std::make_unique<int>(123) };
    property<std::unique_ptr<int>> c = std::move(p);

    ASSERT_EQ(*c.value(), 123);
}

TEST_F(property_test, move_constructed_property_keeps_subscribers)
{
    auto call_count = 0;

    property<std::unique_ptr<int>, property_test> p { std::make_unique<int>(123) };
    p.subscribe([&]() { ++call_count; }).release();

    auto c { std::move(p) };
    set(c, std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

TEST_F(property_test, move_assigned_property_keeps_subscribers)
{
    auto call_count = 0;

    property<std::unique_ptr<int>, property_test> p { std::make_unique<int>(123) };
    p.subscribe([&]() { ++call_count; }).release();

    auto c = std::move(p);
    set(c, std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

} }
