#include <catch/catch.hpp>
#include <observable/value.hpp>
#include <observable/expressions/tree.hpp>

namespace observable { inline namespace expr { namespace test {

TEST_CASE("expression tree/basic node evaluation", "[expression tree]")
{
    SECTION("constant node can be evaluated")
    {
        auto node = expression_node<int> { 5 };
        node.eval();

        REQUIRE(node.get() == 5);
    }

    SECTION("value node can be evaluated")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };
        node.eval();

        REQUIRE(node.get() == 5);
    }

    SECTION("change is reflected in value node")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };
        val = 7;
        node.eval();

        REQUIRE(node.get() == 7);
    }

    SECTION("change is not reflected in value node without calling eval()")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };
        val = 7;

        REQUIRE(node.get() == 5);
    }

    SECTION("unary node can be evaluated")
    {
        auto node = expression_node<int> {
            [](auto v) { return v * 2; },
            expression_node<int> { 5 }
        };
        node.eval();

        REQUIRE(node.get() == 5 * 2);
    }

    SECTION("change is reflected in unary node")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> {
            [](auto v) { return v * 2; },
            expression_node<int> { val }
        };
        val = 7;
        node.eval();

        REQUIRE(node.get() == 7 * 2);
    }

    SECTION("binary node can be evaluated")
    {
        auto node = expression_node<int> {
            [](auto a, auto b) { return a + b; },
            expression_node<int> { 5 },
            expression_node<int> { 7 }
        };
        node.eval();

        REQUIRE(node.get() == 5 + 7);
    }

    SECTION("change is reflected in binary node")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> {
            [](auto a, auto b) { return a + b; },
            expression_node<int> { val },
            expression_node<int> { 7 }
        };
        val = 3;
        node.eval();

        REQUIRE(node.get() == 3 + 7);
    }

    SECTION("enclose value node can be evaluated")
    {
        struct enclosed {
            value<int, enclosed> val { 5 };
        } enc;

        auto node = expression_node<int> { enc.val };
        node.eval();

        REQUIRE(node.get() == 5);
    }
}

TEST_CASE("expression tree/nodes are evaluated only if dirty", "[expression tree]")
{
    SECTION("unary node only evaluates operation if dirty")
    {
        auto call_count = 0;
        auto node = expression_node<int> {
            [&](auto val) { ++call_count; return val; },
            expression_node<int> { 5 }
        };

        node.eval();
        node.eval();

        REQUIRE(call_count == 1);
    }

    SECTION("binary node only evaluates operation if dirty")
    {
        auto call_count = 0;
        auto node = expression_node<int> {
            [&](auto a, auto b) { ++call_count; return a + b; },
            expression_node<int> { 5 },
            expression_node<int> { 7 }
        };

        node.eval();
        node.eval();

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("expression tree/nodes and values are safe to move", "[expression tree]")
{
    SECTION("constant node can be evaluated after move")
    {
        auto node = expression_node<int> { 5 };
        auto new_node = std::move(node);
        new_node.eval();

        REQUIRE(new_node.get() == 5);
    }

    SECTION("value node can be evaluated after node move")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };
        auto new_node = std::move(node);
        new_node.eval();

        REQUIRE(new_node.get() == 5);
    }

    SECTION("value node can be evaluated after value move")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };

        auto moved_val = std::move(val);

        node.eval();
        REQUIRE(node.get() == 5);

        moved_val = 7;
        node.eval();

        REQUIRE(node.get() == 7);
    }

    SECTION("value node can be evaluated after both value and node move")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };

        auto moved_val = std::move(val);
        auto moved_node = std::move(node);

        moved_val = 7;
        moved_node.eval();

        REQUIRE(moved_node.get() == 7);
    }

    SECTION("change is reflected in value node after move")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };
        auto new_node = std::move(node);
        val = 7;
        new_node.eval();

        REQUIRE(new_node.get() == 7);
    }

    SECTION("unary node can be evaluated after move")
    {
        auto node = expression_node<int> {
            [](auto v) { return v * 2; },
            expression_node<int> { 5 }
        };

        auto new_node = std::move(node);
        new_node.eval();

        REQUIRE(new_node.get() == 5 * 2);
    }

    SECTION("binary node can be evaluated after move")
    {
        auto node = expression_node<int> {
            [](auto a, auto b) { return a + b; },
            expression_node<int> { 5 },
            expression_node<int> { 7 }
        };

        auto new_node = std::move(node);
        new_node.eval();

        REQUIRE(new_node.get() == 5 + 7);
    }

    SECTION("change is reflected in binary node after move")
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

        REQUIRE(new_node.get() == 3 + 7);
    }

    SECTION("change is reflected in unary node after move")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> {
            [](auto v) { return v * 2; },
            expression_node<int> { val }
        };

        auto new_node = std::move(node);
        val = 7;
        new_node.eval();

        REQUIRE(new_node.get() == 7 * 2);
    }
}

TEST_CASE("expression tree/value node can be evaluated after value is dead", "[expression tree]")
{
    auto node = expression_node<int> { };

    {
        auto val = value<int> { 5 };
        node = expression_node<int> { val };
        val = 7;
    }

    node.eval();

    REQUIRE(node.get() == 5);
}

TEST_CASE("expression tree/notifications", "[expression tree]")
{
    SECTION("change triggers value node notification")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };

        auto called = false;
        node.subscribe([&]() { called = true; }).release();
        val = 7;

        REQUIRE(called);
    }

    SECTION("change triggers value node notification after move")
    {
        auto val = value<int> { 5 };
        auto node = expression_node<int> { val };

        auto called = false;
        node.subscribe([&]() { called = true; }).release();

        auto new_node = std::move(node);
        val = 7;

        REQUIRE(called);
    }
}

} } }
