#include <memory>
#include <catch/catch.hpp>
#include <observable/expressions/expression.hpp>

namespace observable { inline namespace expr { namespace test {

TEST_CASE("expression/expression creation", "[expression]")
{
    SECTION("can create immediate-update expression")
    {
        auto e = expression<int, immediate_evaluator> { expression_node<int> { 5 } };

        REQUIRE(e.get() == 5);
    }

    SECTION("can create manual-update expression")
    {
        auto ev = expression_evaluator { };
        auto e = expression<int> { expression_node<int> { 5 }, ev };

        REQUIRE(e.get() == 5);
    }
}

TEST_CASE("expression/interactions with values", "[expression]")
{
    SECTION("can get immediate-update expression's value")
    {
        auto e = expression<int, immediate_evaluator> { expression_node<int> { 5 } };

        REQUIRE(e.get() == 5);
    }

    SECTION("can get manual-update expression's value")
    {
        auto ev = expression_evaluator { };
        auto e = expression<int> { expression_node<int> { 5 }, ev };

        REQUIRE(e.get() == 5);
    }

    SECTION("can create value from expression")
    {
        auto val = value<int> {
            std::make_unique<expression<int, immediate_evaluator>>(
                expression_node<int> { 5 }
            )
        };

        REQUIRE(val.get() == 5);
    }

    SECTION("expression change updates value")
    {
        auto val1 = value<int> { 5 };
        auto val2 = value<int> {
            std::make_unique<expression<int, immediate_evaluator>>(
                expression_node<int> { val1 }
            )
        };

        val1 = 7;

        REQUIRE(val2.get() == 7);
    }

    SECTION("can convert expression to value")
    {
        auto val1 = value<int> { 5 };
        auto val2 = value<int> {
            std::make_unique<expression<int, immediate_evaluator>>(
                expression_node<int> { val1 }
            )
        };
        val1 = 7;

        REQUIRE(val2.get() == 7);
    }
}

TEST_CASE("expression/expression updating", "[expression]")
{
    SECTION("immediate-update expression is updated on change")
    {
        auto val = value<int> { 5 };
        auto e = expression<int, immediate_evaluator> { expression_node<int> { val } };
        val = 7;

        REQUIRE(e.get() == 7);
    }

    SECTION("immediate-update expression can be manually updated")
    {
        auto val = value<int> { 5 };
        auto e = expression<int, immediate_evaluator> { expression_node<int> { val } };
        val = 7;
        e.eval();

        REQUIRE(e.get() == 7);
    }

    SECTION("manual-update expression is not updated on change")
    {
        auto ev = expression_evaluator { };

        auto val = value<int> { 5 };
        auto e = expression<int> { expression_node<int> { val }, ev };
        val = 7;

        REQUIRE(e.get() == 5);
    }

    SECTION("manual-update expression can be updated")
    {
        auto ev = expression_evaluator { };

        auto val = value<int> { 5 };
        auto e = expression<int> { expression_node<int> { val }, ev };
        val = 7;
        e.eval();

        REQUIRE(e.get() == 7);
    }

    SECTION("can globally update arbitrary empty evaluator")
    {
        auto ev = expression_evaluator { };
        ev.eval_all();
    }

    SECTION("can globally update expression")
    {
        auto ev = expression_evaluator { };

        auto val = value<int> { 5 };
        auto e = expression<int> { expression_node<int> { val }, ev };
        val = 7;
        ev.eval_all();

        REQUIRE(e.get() == 7);
    }

    SECTION("can globally update multiple expressions")
    {
        auto ev = expression_evaluator { };

        auto val1 = value<int> { 5 };
        auto e1 = expression<int> { expression_node<int> { val1 }, ev };

        auto val2 = value<int> { 13 };
        auto e2 = expression<int> { expression_node<int> { val2 }, ev };

        val1 = 7;
        val2 = 17;

        ev.eval_all();

        REQUIRE(e1.get() == 7);
        REQUIRE(e2.get() == 17);
    }
}

} } }
