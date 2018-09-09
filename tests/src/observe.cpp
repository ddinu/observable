#include <catch/catch.hpp>
#include <observable/observe.hpp>
#include <observable/value.hpp>

namespace observable { namespace test {

TEST_CASE("observe/single value evaluation", "[observe]")
{
    SECTION("immediate update value initialization works")
    {
        auto val = value<int> { 5 };
        auto result = observe(val);

        REQUIRE(result.get() == 5);
    }

    SECTION("enclosed immediate update value initialization works")
    {
        struct enclosed {
            value<int, enclosed> val { 5 };
        } enc;

        auto result = observe(enc.val);

        REQUIRE(result.get() == 5);
    }

    SECTION("single value with immediate update reflects value changes")
    {
        auto val = value<int> { 5 };
        auto result = observe(val);
        val = 7;

        REQUIRE(result.get() == 7);
    }

    SECTION("single value with manual update")
    {
        auto test_updater = updater { };
        auto val = value<int> { 5 };
        auto result = observe(test_updater, val);

        REQUIRE(result.get() == 5);
    }

    SECTION("enclosed single value with manual update")
    {
        auto test_updater = updater { };

        struct enclosed {
            value<int, enclosed> val { 5 };
        } enc;

        auto result = observe(test_updater, enc.val);

        REQUIRE(result.get() == 5);
    }

    SECTION("single value with manual update propagates changes")
    {
        auto test_updater = updater { };
        auto val = value<int> { 5 };
        auto result = observe(test_updater, val);

        val = 7;
        test_updater.update_all();

        REQUIRE(result.get() == 7);
    }

    SECTION("single value with manual update does not change without calling eval()")
    {
        auto test_updater = updater { };
        auto val = value<int> { 5 };
        auto result = observe(test_updater, val);
        val = 7;

        REQUIRE(result.get() == 5);
    }
}

TEST_CASE("observe/single value change notifications", "[observe]")
{
    SECTION("single value with immediate update triggers change notification")
    {
        auto val = value<int> { 1 };
        auto result = observe(val);

        auto call_count = 0;
        result.subscribe([&]() { ++call_count; }).release();
        val = 10;

        REQUIRE(call_count == 1);
    }

    SECTION("single value with manual update triggers change notification on eval")
    {
        auto test_updater = updater { };
        auto val = value<int> { 1 };
        auto result = observe(test_updater, val);

        auto call_count = 0;
        result.subscribe([&]() { ++call_count; }).release();
        val = 10;
        test_updater.update_all();

        REQUIRE(call_count == 1);
    }

    SECTION("single value with manual update does not trigger notification without eval")
    {
        auto test_updater = updater { };
        auto val = value<int> { 1 };
        auto result = observe(test_updater, val);

        auto call_count = 0;
        result.subscribe([&]() { ++call_count; }).release();
        val = 10;

        REQUIRE(call_count == 0);
    }
}

TEST_CASE("observe/expression evaluation", "[observe]")
{
    SECTION("chained values with immediate update are updated")
    {
        auto a = value<int> { 1 };
        auto v1 = observe(a);
        auto v2 = observe(v1);

        a = 3;

        REQUIRE(v2.get() == 3);
    }

    SECTION("chained expressions with immediate update are updated")
    {
        auto a = value<int> { 1 };
        auto v1 = observe(a + 2);
        auto v2 = observe(v1 + 2);

        a = 3;

        REQUIRE(v2.get() == 7);
    }

    SECTION("chained values with manual update are updated")
    {
        auto test_updater = updater { };

        auto a = value<int> { 1 };
        auto v1 = observe(test_updater, a);
        auto v2 = observe(test_updater, v1);

        a = 3;
        test_updater.update_all();

        REQUIRE(v2.get() == 3);
    }

    SECTION("chained expressions with manual update are updated")
    {
        auto test_updater = updater { };

        auto a = value<int> { 1 };
        auto v1 = observe(test_updater, a + 2);
        auto v2 = observe(test_updater, v1 + 2);

        a = 3;
        test_updater.update_all();

        REQUIRE(v2.get() == 7);
    }
}

TEST_CASE("observe/expression change notifications", "[observe]")
{
    SECTION("expression with immediate update triggers change notification")
    {
        auto a = value<int> { 1 };
        auto b = value<int> { 2 };
        auto result = observe((a + b) / 2);

        auto call_count = 0;
        result.subscribe([&]() { ++call_count; }).release();

        a = 10;
        b = 20;

        REQUIRE(call_count == 2);
    }

    SECTION("expression with manual update triggers change notification on eval")
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

        REQUIRE(call_count == 1);
    }

    SECTION("expression with manual update does not trigger change notification without eval")
    {
        auto test_updater = updater { };
        auto a = value<int> { 1 };
        auto b = value<int> { 2 };
        auto result = observe(test_updater, (a + b) / 2);

        auto call_count = 0;
        result.subscribe([&]() { ++call_count; }).release();

        a = 10;
        b = 20;

        REQUIRE(call_count == 0);
    }
}

TEST_CASE("observe/expssion works after everything gets moved", "[observe]")
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

    REQUIRE(r.get() == Approx { (10 + 30) / 2.0 });
}

} }
