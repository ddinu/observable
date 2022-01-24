#include <functional>
#include <catch/catch.hpp>
#include <observable/value.hpp>
#include <observable/expressions/operators.hpp>
#include <observable/expressions/tree.hpp>

namespace { // Operators should be usable with ADL.

using observable::value;
using observable::expression_node;

#define MAKE_UNARY_OP_TEST(NAME, OP, V) \
TEST_CASE("expression operator/" NAME, "[expression operator]") \
{ \
    auto v = value<decltype(V)> { V }; \
\
    auto const expected = OP V; \
\
    REQUIRE((OP v).get() == expected); \
    REQUIRE((OP expression_node<decltype(V)> { V }).get() == expected); \
\
    struct enclosing { \
        value<decltype(V), enclosing> v { V }; \
    } enc ; \
\
    REQUIRE((OP enc.v).get() == expected); \
}

MAKE_UNARY_OP_TEST("!", !, false)
MAKE_UNARY_OP_TEST("~", ~, 0xb0101)
MAKE_UNARY_OP_TEST("unary +", +, 5)
MAKE_UNARY_OP_TEST("unary -", -, 5)

#define MAKE_BINARY_OP_TEST(NAME, A, OP, B) \
TEST_CASE("expression operator/" NAME, "[expression operator]") \
{ \
    auto va = value<decltype(A)> { A }; \
    auto vb = value<decltype(B)> { B }; \
\
    auto const expected = A OP B; \
\
    REQUIRE((va OP B).get() == expected); \
    REQUIRE((A OP vb).get() == expected); \
    REQUIRE((va OP vb).get() == expected); \
    REQUIRE((expression_node<decltype(A)> { A } OP B).get() == expected); \
    REQUIRE((A OP expression_node<decltype(B)> { B }).get() == expected); \
    REQUIRE((expression_node<decltype(A)> { A } OP expression_node<decltype(B)> { B }).get() == expected); \
    REQUIRE((va OP expression_node<decltype(B)> { B }).get() == expected); \
    REQUIRE((expression_node<decltype(A)> { A } OP vb).get() == expected); \
\
    struct enclosing { \
        value<decltype(A), enclosing> va { A }; \
        value<decltype(B), enclosing> vb { B }; \
    } enc; \
\
    REQUIRE((enc.va OP B).get() == expected); \
    REQUIRE((A OP enc.vb).get() == expected); \
    REQUIRE((enc.va OP enc.vb).get() == expected); \
    REQUIRE((enc.va OP expression_node<decltype(B)> { B }).get() == expected); \
    REQUIRE((expression_node<decltype(A)> { A } OP enc.vb).get() == expected); \
}

MAKE_BINARY_OP_TEST("*", 5, *, 7)
MAKE_BINARY_OP_TEST("/", 10, /, 2)
MAKE_BINARY_OP_TEST("%", 5, %, 2)
MAKE_BINARY_OP_TEST("+", 5, +, 3)
MAKE_BINARY_OP_TEST("-", 5, -, 3)
MAKE_BINARY_OP_TEST("<<", 0x1, <<, 2)
MAKE_BINARY_OP_TEST(">>", 0xF0, >>, 2)
MAKE_BINARY_OP_TEST("<", 2, <, 5)
MAKE_BINARY_OP_TEST("<=", 2, <=, 5)
MAKE_BINARY_OP_TEST(">", 5, >, 2)
MAKE_BINARY_OP_TEST(">=", 5, >=, 2)
MAKE_BINARY_OP_TEST("==", 7, ==, 7)
MAKE_BINARY_OP_TEST("!=", 7, !=, 3)
MAKE_BINARY_OP_TEST("&", 0x1, &, 0x1)
MAKE_BINARY_OP_TEST("^", 0x1, ^, 0x2)
MAKE_BINARY_OP_TEST("|", 0x1, |, 0x2)
MAKE_BINARY_OP_TEST("&&", true, &&, true)
MAKE_BINARY_OP_TEST("||", true, ||, false)

TEST_CASE("expression operator/nodes are updated", "[expression operator]")
{
    auto a = value<int> { 5 };
    auto b = value<int> { 9 };
    auto c = value<float> { 10 };

    auto r = ((a + b) * c) / 2.5f;

    a = 7;
    b = 13;
    c = 15;

    r.eval();

    REQUIRE(r.get() == Approx { ((static_cast<float>(a.get()) + static_cast<float>(b.get())) * c.get()) / 2.5f });
}

}
