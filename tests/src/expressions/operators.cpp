#include <functional>
#include "observable/value.hpp"
#include "observable/expressions/operators.hpp"
#include "observable/expressions/tree.hpp"
#include "gtest.h"

using observable::value;
using observable::expression_node;

#define MAKE_UNARY_OP_TEST(NAME, OP, V) \
TEST(expression_operators_test, NAME) \
{ \
    auto v = value<decltype(V)> { V }; \
\
    auto const expected = OP V; \
\
    ASSERT_EQ(expected, (OP v).get()); \
    ASSERT_EQ(expected, (OP expression_node<decltype(V)> { V }).get()); \
\
    struct enclosing { \
        value<decltype(V), std::equal_to<>, enclosing> v { V }; \
    } enc ; \
\
    ASSERT_EQ(expected, (OP enc.v).get()); \
}

MAKE_UNARY_OP_TEST(logical_not, !, false)
MAKE_UNARY_OP_TEST(bitwise_not, ~, 0xb0101)
MAKE_UNARY_OP_TEST(unary_plus, +, 5)
MAKE_UNARY_OP_TEST(unary_minus, -, 5)

#define MAKE_BINARY_OP_TEST(NAME, A, OP, B) \
TEST(expression_operators_test, NAME) \
{ \
    auto va = value<decltype(A)> { A }; \
    auto vb = value<decltype(B)> { B }; \
\
    auto const expected = A OP B; \
\
    ASSERT_EQ(expected, (va OP B).get()); \
    ASSERT_EQ(expected, (A OP vb).get()); \
    ASSERT_EQ(expected, (va OP vb).get()); \
    ASSERT_EQ(expected, (expression_node<decltype(A)> { A } OP B).get()); \
    ASSERT_EQ(expected, (A OP expression_node<decltype(B)> { B }).get()); \
    ASSERT_EQ(expected, (expression_node<decltype(A)> { A } OP expression_node<decltype(B)> { B }).get()); \
    ASSERT_EQ(expected, (va OP expression_node<decltype(B)> { B }).get()); \
    ASSERT_EQ(expected, (expression_node<decltype(A)> { A } OP vb).get()); \
\
    struct enclosing { \
        value<decltype(A), std::equal_to<>, enclosing> va { A }; \
        value<decltype(B), std::equal_to<>, enclosing> vb { B }; \
    } enc; \
\
    ASSERT_EQ(expected, (enc.va OP B).get()); \
    ASSERT_EQ(expected, (A OP enc.vb).get()); \
    ASSERT_EQ(expected, (enc.va OP enc.vb).get()); \
    ASSERT_EQ(expected, (enc.va OP expression_node<decltype(B)> { B }).get()); \
    ASSERT_EQ(expected, (expression_node<decltype(A)> { A } OP enc.vb).get()); \
}

MAKE_BINARY_OP_TEST(multiplication, 5, *, 7)
MAKE_BINARY_OP_TEST(division, 10, /, 2)
MAKE_BINARY_OP_TEST(modulo, 5, %, 2)
MAKE_BINARY_OP_TEST(addition, 5, +, 3)
MAKE_BINARY_OP_TEST(subtraction, 5, -, 3)
MAKE_BINARY_OP_TEST(left_shift, 0x1, <<, 2)
MAKE_BINARY_OP_TEST(right_shift, 0xF0, >>, 2)
MAKE_BINARY_OP_TEST(less_than, 2, <, 5)
MAKE_BINARY_OP_TEST(less_than_equal, 2, <=, 5)
MAKE_BINARY_OP_TEST(greather_than, 5, >, 2)
MAKE_BINARY_OP_TEST(greather_than_equal, 5, >=, 2)
MAKE_BINARY_OP_TEST(equal, 7, ==, 7)
MAKE_BINARY_OP_TEST(not_equal, 7, !=, 3)
MAKE_BINARY_OP_TEST(bitwise_and, 0x1, &, 0x1)
MAKE_BINARY_OP_TEST(bitwise_xor, 0x1, ^, 0x2)
MAKE_BINARY_OP_TEST(bitwise_or, 0x1, |, 0x2)
MAKE_BINARY_OP_TEST(and, true, &&, true)
MAKE_BINARY_OP_TEST(or, true, ||, false)

TEST(expression_operators_test, node_is_updated)
{
    auto a = value<int> { 5 };
    auto b = value<int> { 9 };
    auto c = value<float> { 10 };

    auto r = ((a + b) * c) / 2.5f;

    a = 7;
    b = 13;
    c = 15;

    r.eval();

    ASSERT_FLOAT_EQ(((a.get() + b.get()) * c.get()) / 2.5f, r.get());
}
