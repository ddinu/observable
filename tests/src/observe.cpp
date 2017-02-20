#include "observable/observe.hpp"
#include "observable/value.hpp"
#include "gtest.h"

namespace observable { namespace test {

TEST(observe_test, single_value_with_immediate_update)
{
    auto val = value<int> { 5 };
    auto result = observe(val);

    ASSERT_EQ(5, result.get());
}

TEST(observe_test, single_enclosed_value_with_immediate_update)
{
    struct enclosed {
        value<int, enclosed> val { 5 };
    } enc;

    auto result = observe(enc.val);

    ASSERT_EQ(5, result.get());
}

TEST(observe_test, single_value_with_manual_update)
{
    auto test_updater = updater { };
    auto val = value<int> { 5 };
    auto result = observe(test_updater, val);

    ASSERT_EQ(5, result.get());
}

TEST(observe_test, single_enclosed_value_with_manual_update)
{
    auto test_updater = updater { };

    struct enclosed {
        value<int, enclosed> val { 5 };
    } enc;

    auto result = observe(test_updater, enc.val);

    ASSERT_EQ(5, result.get());
}

TEST(observe_test, single_value_with_immediate_update_propagates_changes)
{
    auto val = value<int> { 5 };
    auto result = observe(val);
    val = 7;

    ASSERT_EQ(7, result.get());
}

TEST(observe_test, single_value_with_manual_update_propagates_changes)
{
    auto test_updater = updater { };
    auto val = value<int> { 5 };
    auto result = observe(test_updater, val);

    val = 7;
    test_updater.update_all();

    ASSERT_EQ(7, result.get());
}

TEST(observe_test, single_value_with_manual_update_does_not_change_without_eval)
{
    auto test_updater = updater { };
    auto val = value<int> { 5 };
    auto result = observe(test_updater, val);
    val = 7;

    ASSERT_EQ(5, result.get());
}

TEST(observe_test, single_value_with_immediate_update_triggers_change_notification)
{
    auto val = value<int> { 1 };
    auto result = observe(val);

    auto call_count = 0;
    result.subscribe([&]() { ++call_count; }).release();
    val = 10;

    ASSERT_EQ(1, call_count);
}

TEST(observe_test, single_value_with_manual_update_triggers_change_notification_on_eval)
{
    auto test_updater = updater { };
    auto val = value<int> { 1 };
    auto result = observe(test_updater, val);

    auto call_count = 0;
    result.subscribe([&]() { ++call_count; }).release();
    val = 10;
    test_updater.update_all();

    ASSERT_EQ(1, call_count);
}

TEST(observe_test, single_value_with_manual_update_does_not_trigger_notification_without_eval)
{
    auto test_updater = updater { };
    auto val = value<int> { 1 };
    auto result = observe(test_updater, val);

    auto call_count = 0;
    result.subscribe([&]() { ++call_count; }).release();
    val = 10;

    ASSERT_EQ(0, call_count);
}

TEST(observe_test, expression_with_immediate_update)
{
    auto a = value<int> { 10 };
    auto b = value<int> { 20 };
    auto result = observe((a + b) / 2);

    ASSERT_EQ(15, result.get());
}

TEST(observe_test, expression_with_manual_update)
{
    auto test_updater = updater { };

    auto a = value<int> { 10 };
    auto b = value<int> { 20 };
    auto result = observe(test_updater, (a + b) / 2);

    ASSERT_EQ(15, result.get());
}

TEST(observe_test, expression_with_immediate_update_propagates_changes)
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto result = observe((a + b) / 2);

    a = 10;
    b = 20;

    ASSERT_EQ(15, result.get());
}

TEST(observe_test, expression_with_manual_update_propagates_changes)
{
    auto test_updater = updater { };

    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto result = observe(test_updater, (a + b) / 2);

    a = 10;
    b = 20;
    test_updater.update_all();

    ASSERT_EQ(15, result.get());
}

TEST(observe_test, expression_with_manual_update_does_not_change_without_eval)
{
    auto test_updater = updater { };

    auto a = value<int> { 10 };
    auto b = value<int> { 20 };
    auto result = observe(test_updater, (a + b) / 2);

    a = 1;
    b = 2;

    ASSERT_EQ(15, result.get());
}

TEST(observe_test, expression_with_immediate_update_triggers_change_notification)
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto result = observe((a + b) / 2);

    auto call_count = 0;
    result.subscribe([&]() { ++call_count; }).release();

    a = 10;
    b = 20;

    ASSERT_EQ(2, call_count);
}

TEST(observe_test, expression_with_manual_update_triggers_change_notification_on_eval)
{
    auto test_updater = updater { };
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto result = observe(test_updater, (a + b) / 2);

    auto call_count = 0;
    result.subscribe([&]() { ++call_count; }).release();

    a = 10;
    b = 20;
    test_updater.update_all();

    ASSERT_EQ(1, call_count);
}

TEST(observe_test, expression_with_manual_update_does_not_trigger_change_notification_without_eval)
{
    auto test_updater = updater { };
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto result = observe(test_updater, (a + b) / 2);

    auto call_count = 0;
    result.subscribe([&]() { ++call_count; }).release();

    a = 10;
    b = 20;

    ASSERT_EQ(0, call_count);
}

TEST(observe_test, chained_values_with_immediate_update_are_updated)
{
    auto a = value<int> { 1 };
    auto v1 = observe(a);
    auto v2 = observe(v1);

    a = 3;

    ASSERT_EQ(3, v2.get());
}

TEST(observe_test, chained_expressions_with_immediate_update_are_updated)
{
    auto a = value<int> { 1 };
    auto v1 = observe(a + 2);
    auto v2 = observe(v1 + 2);

    a = 3;

    ASSERT_EQ(7, v2.get());
}

TEST(observe_test, chained_values_with_manual_update_are_updated)
{
    auto test_updater = updater { };

    auto a = value<int> { 1 };
    auto v1 = observe(test_updater, a);
    auto v2 = observe(test_updater, v1);

    a = 3;
    test_updater.update_all();

    ASSERT_EQ(3, v2.get());
}

TEST(observe_test, chained_expressions_with_manual_update_are_updated)
{
    auto test_updater = updater { };

    auto a = value<int> { 1 };
    auto v1 = observe(test_updater, a + 2);
    auto v2 = observe(test_updater, v1 + 2);

    a = 3;
    test_updater.update_all();

    ASSERT_EQ(7, v2.get());
}

TEST(observe_test, expssion_works_after_everythin_gets_moved)
{
    auto a = value<int> { };
    auto b = value<int> { };
    auto r = value<double> { };

    {
        auto a1 = value<int> { 2 };
        auto b1 = value<int> { 3 };
        auto r1 = observe((a1 + b1) / 2.0);

        a = std::move(a1);
        b = std::move(b1);
        r = std::move(r1);
    }

    a = 10;
    b = 30;

    ASSERT_EQ((10 + 30) / 2.0, r.get());
}

} }
