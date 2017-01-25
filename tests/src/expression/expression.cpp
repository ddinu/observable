#include <memory>
#include "observable/expression/expression.hpp"
#include "gtest.h"

namespace observable { inline namespace expr { namespace test {

TEST(expression_test, can_create_immediate_expression)
{
    auto e = expression<int, immediate_evaluator> { expression_node<int> { 5 } };
}

TEST(expression_test, can_create_manual_update_expression)
{
    auto ev = expression_evaluator { };
    auto e = expression<int> { expression_node<int> { 5 }, ev };
}

TEST(expression_test, can_get_immediate_expression_value)
{
    auto e = expression<int, immediate_evaluator> { expression_node<int> { 5 } };

    ASSERT_EQ(5, e.get());
}

TEST(expression_test, can_get_manual_update_expression_value)
{
    auto ev = expression_evaluator { };
    auto e = expression<int> { expression_node<int> { 5 }, ev };

    ASSERT_EQ(5, e.get());
}

TEST(expression_test, immediate_update_expression_is_updated_on_change)
{
    auto val = value<int> { 5 };
    auto e = expression<int, immediate_evaluator> { expression_node<int> { val } };
    val = 7;

    ASSERT_EQ(7, e.get());
}

TEST(expression_test, immediate_update_expression_can_be_manually_updated)
{
    auto val = value<int> { 5 };
    auto e = expression<int, immediate_evaluator> { expression_node<int> { val } };
    val = 7;
    e.eval();
}

TEST(expression_test, manual_update_expression_is_not_updated_on_change)
{
    auto ev = expression_evaluator { };

    auto val = value<int> { 5 };
    auto e = expression<int> { expression_node<int> { val }, ev };
    val = 7;

    ASSERT_EQ(5, e.get());
}

TEST(expression_test, manual_update_expression_can_be_updated)
{
    auto ev = expression_evaluator { };

    auto val = value<int> { 5 };
    auto e = expression<int> { expression_node<int> { val }, ev };
    val = 7;
    e.eval();

    ASSERT_EQ(7, e.get());
}

TEST(expression_test, can_globally_update_arbitrary_empty_evaluator)
{
    auto ev = expression_evaluator { };
    ev.eval_all();
}

TEST(expression_test, can_globally_update_expression)
{
    auto ev = expression_evaluator { };

    auto val = value<int> { 5 };
    auto e = expression<int> { expression_node<int> { val }, ev };
    val = 7;
    ev.eval_all();

    ASSERT_EQ(7, e.get());
}

TEST(expression_test, can_globally_update_multiple_expressions)
{
    auto ev = expression_evaluator { };

    auto val1 = value<int> { 5 };
    auto e1 = expression<int> { expression_node<int> { val1 }, ev };

    auto val2 = value<int> { 13 };
    auto e2 = expression<int> { expression_node<int> { val2 }, ev };

    val1 = 7;
    val2 = 17;

    ev.eval_all();

    ASSERT_EQ(7, e1.get());
    ASSERT_EQ(17, e2.get());
}

TEST(expression_test, can_create_value_from_expression)
{
    auto val = value<int> {
                   std::make_unique<expression<int, immediate_evaluator>>(
                       expression_node<int> { 5 }
                   )
               };

    ASSERT_EQ(5, val.get());
}

TEST(expression_test, expression_change_updates_value)
{
    auto val1 = value<int> { 5 };
    auto val2 = value<int> {
                    std::make_unique<expression<int, immediate_evaluator>>(
                        expression_node<int> { val1 }
                    )
                };

    val1 = 7;

    ASSERT_EQ(7, val2.get());
}

TEST(expression_test, can_convert_expression_to_value)
{
    auto val1 = value<int> { 5 };
    auto val2 = value<int> {
                    std::make_unique<expression<int, immediate_evaluator>>(
                        expression_node<int> { val1 }
                    )
                };
    val1 = 7;

    ASSERT_EQ(7, val2.get());
}

} } }
