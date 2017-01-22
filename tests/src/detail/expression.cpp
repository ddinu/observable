#include <memory>
#include <utility>
#include "gtest.h"
#include "observable/detail/expression.hpp"

namespace observable { namespace detail { namespace test {

TEST(expression_test, can_create_immediate_expression)
{
    auto e = expression<int, immediate_update_tag> { expression_node<int> { 5 } };
}

TEST(expression_test, can_create_manual_update_expression)
{
    struct some_tag : expression_updater { } tag;
    auto e = expression<int, some_tag> { expression_node<int> { 5 }, tag };
}

TEST(expression_test, can_get_immediate_expression_value)
{
    auto e = expression<int, immediate_update_tag> { expression_node<int> { 5 } };

    ASSERT_EQ(5, e.get());
}

TEST(expression_test, can_get_manual_update_expression_value)
{
    struct some_tag : expression_updater { } tag;
    auto e = expression<int, some_tag> { expression_node<int> { 5 }, tag };

    ASSERT_EQ(5, e.get());
}

TEST(expression_test, immediate_update_expression_is_updated_on_change)
{
    auto val = value<int> { 5 };
    auto e = expression<int, immediate_update_tag> { expression_node<int> { val } };
    val = 7;

    ASSERT_EQ(7, e.get());
}

TEST(expression_test, immediate_update_expression_can_be_manually_updated)
{
    auto val = value<int> { 5 };
    auto e = expression<int, immediate_update_tag> { expression_node<int> { val } };
    val = 7;
    e.eval();
}

TEST(expression_test, manual_update_expression_is_not_updated_on_change)
{
    struct some_tag : expression_updater { } tag;

    auto val = value<int> { 5 };
    auto e = expression<int, some_tag> { expression_node<int> { val }, tag };
    val = 7;

    ASSERT_EQ(5, e.get());
}

TEST(expression_test, manual_update_expression_can_be_updated)
{
    struct some_tag : expression_updater { } tag;

    auto val = value<int> { 5 };
    auto e = expression<int, some_tag> { expression_node<int> { val }, tag };
    val = 7;
    e.eval();

    ASSERT_EQ(7, e.get());
}

TEST(expression_test, can_globally_update_arbitrary_empty_tag)
{
    struct some_tag : expression_updater { } tag;
    tag.eval_all();
}

TEST(expression_test, can_globally_update_expression)
{
    struct some_tag : expression_updater { } tag;

    auto val = value<int> { 5 };
    auto e = expression<int, some_tag> { expression_node<int> { val }, tag };
    val = 7;
    tag.eval_all();

    ASSERT_EQ(7, e.get());
}

TEST(expression_test, can_globally_update_multiple_expressions)
{
    struct some_tag : expression_updater { } tag;

    auto val1 = value<int> { 5 };
    auto e1 = expression<int, some_tag> { expression_node<int> { val1 }, tag };

    auto val2 = value<int> { 13 };
    auto e2 = expression<int, some_tag> { expression_node<int> { val2 }, tag };

    val1 = 7;
    val2 = 17;

    tag.eval_all();

    ASSERT_EQ(7, e1.get());
    ASSERT_EQ(17, e2.get());
}

TEST(expression_test, can_create_value_from_expression)
{
    auto val = value<int> {
                   std::make_unique<expression<int, immediate_update_tag>>(
                       expression_node<int> { 5 }
                   )
               };

    ASSERT_EQ(5, val.get());
}

TEST(expression_test, expression_change_updates_value)
{
    auto val1 = value<int> { 5 };
    auto val2 = value<int> {
                    std::make_unique<expression<int, immediate_update_tag>>(
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
                    std::make_unique<expression<int, immediate_update_tag>>(
                        expression_node<int> { val1 }
                    )
                };
    val1 = 7;

    ASSERT_EQ(7, val2.get());
}

} } }
