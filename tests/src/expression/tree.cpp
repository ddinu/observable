#include "observable/value.hpp"
#include "observable/expression/tree.hpp"
#include "gtest.h"

namespace observable { namespace detail { namespace test {

TEST(expression_tree_test, constant_node_can_be_evaluated)
{
    auto node = expression_node<int> { 5 };
    node.eval();

    ASSERT_EQ(5, node.get());
}

TEST(expression_tree_test, constant_node_can_be_evaluated_after_move)
{
    auto node = expression_node<int> { 5 };
    auto new_node = std::move(node);
    new_node.eval();

    ASSERT_EQ(5, new_node.get());
}

TEST(expression_tree_test, value_node_can_be_evaluated)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> { val };
    node.eval();

    ASSERT_EQ(5, node.get());
}

TEST(expression_tree_test, value_node_can_be_evaluated_after_move)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> { val };
    auto new_node = std::move(node);
    new_node.eval();

    ASSERT_EQ(5, new_node.get());
}

TEST(expression_tree_test, value_node_can_be_evaluated_after_value_is_dead)
{
    auto node = expression_node<int> { };

    {
        auto val = value<int> { 5 };
        node = expression_node<int> { val };
    }

    node.eval();

    ASSERT_EQ(5, node.get());
}

TEST(expression_tree_test, change_is_reflected_in_value_node_after_eval)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> { val };
    val = 7;
    node.eval();

    ASSERT_EQ(7, node.get());
}

TEST(expression_tree_test, change_is_not_reflected_in_value_node_without_eval)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> { val };
    val = 7;

    ASSERT_EQ(5, node.get());
}

TEST(expression_tree_test, change_is_reflected_in_value_node_after_move)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> { val };
    auto new_node = std::move(node);
    val = 7;
    new_node.eval();

    ASSERT_EQ(7, new_node.get());
}

TEST(expression_tree_test, change_triggers_value_node_notificaction)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> { val };

    auto called = false;
    node.subscribe([&]() { called = true; }).release();
    val = 7;

    ASSERT_TRUE(called);
}

TEST(expression_tree_test, change_triggers_value_node_notification_after_move)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> { val };

    auto called = false;
    node.subscribe([&]() { called = true; }).release();

    auto new_node = std::move(node);
    val = 7;

    ASSERT_TRUE(called);
}

TEST(expression_tree_test, unary_node_can_be_evaluated)
{
    auto node = expression_node<int> {
        [](auto v) { return v * 2; },
        expression_node<int> { 5 }
    };
    node.eval();

    ASSERT_EQ(5 * 2, node.get());
}

TEST(expression_tree_test, unary_node_can_be_evaluated_after_move)
{
    auto node = expression_node<int> {
        [](auto v) { return v * 2; },
        expression_node<int> { 5 }
    };

    auto new_node = std::move(node);
    new_node.eval();

    ASSERT_EQ(5 * 2, new_node.get());
}

TEST(expression_tree_test, change_is_reflected_in_unary_node)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> {
        [](auto v) { return v * 2; },
        expression_node<int> { val }
    };
    val = 7;
    node.eval();

    ASSERT_EQ(7 * 2, node.get());
}

TEST(expression_tree_test, change_is_reflected_in_unary_node_after_move)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> {
        [](auto v) { return v * 2; },
        expression_node<int> { val }
    };

    auto new_node = std::move(node);
    val = 7;
    new_node.eval();

    ASSERT_EQ(7 * 2, new_node.get());
}

TEST(expression_tree_test, unary_node_only_evaluates_op_if_dirty)
{
    auto call_count = 0;
    auto node = expression_node<int> {
        [&](auto val) { ++call_count; return val; },
        expression_node<int> { 5 }
    };

    node.eval();
    node.eval();

    ASSERT_EQ(1, call_count);
}

TEST(expression_tree_test, binary_node_can_be_evaluated)
{
    auto node = expression_node<int> {
        [](auto a, auto b) { return a + b; },
        expression_node<int> { 5 },
        expression_node<int> { 7 }
    };
    node.eval();

    ASSERT_EQ(5 + 7, node.get());
}

TEST(expression_tree_test, binary_node_can_be_evaluated_after_move)
{
    auto node = expression_node<int> {
        [](auto a, auto b) { return a + b; },
        expression_node<int> { 5 },
        expression_node<int> { 7 }
    };

    auto new_node = std::move(node);
    new_node.eval();

    ASSERT_EQ(5 + 7, new_node.get());
}

TEST(expression_tree_test, change_is_reflected_in_binary_node)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> {
        [](auto a, auto b) { return a + b; },
        expression_node<int> { val },
        expression_node<int> { 7 }
    };
    val = 3;
    node.eval();

    ASSERT_EQ(3 + 7, node.get());
}

TEST(expression_tree_test, change_is_reflected_in_binary_node_after_move)
{
    auto val = value<int> { 5 };
    auto node = expression_node<int> {
        [](auto a, auto b) { return a + b; },
        expression_node<int> { val },
        expression_node<int> { 7 }
    };

    auto new_node = std::move(node);
    val = 3;
    new_node.eval();

    ASSERT_EQ(3 + 7, new_node.get());
}

TEST(expression_tree_test, binary_node_only_evaluates_op_if_dirty)
{
    auto call_count = 0;
    auto node = expression_node<int> {
        [&](auto a, auto b) { ++call_count; return a + b; },
        expression_node<int> { 5 },
        expression_node<int> { 7 }
    };

    node.eval();
    node.eval();

    ASSERT_EQ(1, call_count);
}

} } }
