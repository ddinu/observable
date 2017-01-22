#include "gtest.h"
#include "observable/observe.hpp"

namespace observable { namespace test {

TEST(observe_test, single_value_with_immediate_update)
{
    auto val = value<int> { 5 };
    auto result = observe(val);

    ASSERT_EQ(5, result.get());
}

TEST(observe_test, single_value_with_manual_update)
{
    auto test_updater = updater { };
    auto val = value<int> { 5 };
    auto result = observe(test_updater, val);

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
    test_updater.eval_all();

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
    test_updater.eval_all();

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
    test_updater.eval_all();

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
    test_updater.eval_all();

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

} }
