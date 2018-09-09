#include <catch/catch.hpp>
#include <observable/observe.hpp>
#include <observable/value.hpp>
#include <observable/expressions/operators.hpp>
#include <observable/expressions/filters.hpp>

namespace { // Filters should be usable with ADL.

using observable::value;

auto test_filter_(int a, int b) { return a + b; }
OBSERVABLE_ADAPT_FILTER(test_filter, test_filter_)

TEST_CASE("filter/adapter_filter", "[filter]")
{
    SECTION("adapter_filter computes initial value")
    {
        auto val = value<int> { 5 };
        auto res = observe(test_filter(val, 2));

        REQUIRE(res.get() == 5 + 2);
    }

    SECTION("adapted_filter recomputes value")
    {
        auto val = value<int> { 5 };
        auto res = observe(test_filter(val, 2));

        val = 7;

        REQUIRE(res.get() == (7 + 2));
    }

    SECTION("adapted_filter can take expression parameters")
    {
        auto a = value<int> { 1 };
        auto b = value<int> { 2 };
        auto c = value<int> { 3 };

        auto res = observe(test_filter(a + b, b + c));

        REQUIRE(res.get() == 1 + 2 + 2 + 3);

        a = 10;
        b = 20;
        c = 30;

        REQUIRE(res.get() == 10 + 20 + 20 + 30);
    }

    SECTION("adapted_filter can participate in expression")
    {
        auto a = value<int> { 1 };
        auto b = value<int> { 2 };
        auto c = value<int> { 3 };

        auto res = observe(a + test_filter(b, c) * 20);

        REQUIRE(res.get() == 1 + (2 + 3) * 20);

        a = 10;
        b = 20;
        c = 30;

        REQUIRE(res.get() == 10 + (20 + 30) * 20);
    }
}

TEST_CASE("filter/select", "[filter]")
{
    auto p = value<bool> { true };
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };

    auto res = observe(select(p, a, b));

    REQUIRE(res.get() == 1);

    a = 10;
    REQUIRE(res.get() == 10);

    b = 20;
    REQUIRE(res.get() == 10);

    p = false;
    REQUIRE(res.get() == 20);

    b = 200;
    REQUIRE(res.get() == 200);
}

TEST_CASE("filter/min", "[filter]")
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(min(a, b, c));

    REQUIRE(res.get() == 1);

    a = 50;
    b = 30;
    c = 40;

    REQUIRE(res.get() == 30);
}

TEST_CASE("filter/max", "[filter]")
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(max(a, b, c));

    REQUIRE(res.get() == 3);

    a = 50;
    b = 30;
    c = 40;

    REQUIRE(res.get() == 50);
}

TEST_CASE("filter/mean", "[filter]")
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(mean(a, b, c));

    REQUIRE(res.get() == (1 + 2 + 3) / 3.0);

    a = 10;
    b = 20;
    c = 30;

    REQUIRE(res.get() == (10 + 20 + 30) / 3.0);
}

TEST_CASE("filter/clamp", "[filter]")
{
    auto val = value<int> { 2 };
    auto low = value<int> { 1 };
    auto high = value<int> { 3 };

    auto res = observe(clamp(val, low, high));

    REQUIRE(res.get() == 2);

    val = 30;
    REQUIRE(res.get() == 3);

    high = 40;
    REQUIRE(res.get() == 30);

    low = 35;
    REQUIRE(res.get() == 35);
}

TEST_CASE("filter/zip", "[filter]")
{
    auto a = value<int> { 1 };
    auto b = value<int> { 2 };
    auto c = value<int> { 3 };

    auto res = observe(zip(a, b, c));

    using std::get;

    REQUIRE(get<0>(res.get()) == 1);
    REQUIRE(get<1>(res.get()) == 2);
    REQUIRE(get<2>(res.get()) == 3);

    a = 10;
    b = 20;
    c = 30;

    REQUIRE(get<0>(res.get()) == 10);
    REQUIRE(get<1>(res.get()) == 20);
    REQUIRE(get<2>(res.get()) == 30);
}

TEST_CASE("filter/construct", "[filter]")
{
    struct mock
    {
        int ma { 0 };
        int mb { 0 };

        mock() =default;

        mock(int a, int b) : ma { a }, mb { b }
        { }
    };

    auto a = value<int> { 5 };
    auto res = observe(observable::construct<mock>(a, 5));

    REQUIRE(res.get().ma == 5);
    REQUIRE(res.get().mb == 5);

    a = 7;

    REQUIRE(res.get().ma == 7);
    REQUIRE(res.get().mb == 5);
}

TEST_CASE("filter/static_expr_cast", "[filter]")
{
    struct mock
    {
        mock() =default;

        explicit mock(int a) : a_ { a }
        { }

        explicit operator int() { return a_; }

    private:
        int a_ { 0 };
    };

    auto a = value<mock> { mock { 5 } };
    auto res = observe(observable::static_expr_cast<int>(a));

    REQUIRE(res.get() == 5);

    a = mock { 7 };

    REQUIRE(res.get() == 7);
}

TEST_CASE("filter/reinterpret_expr_cast", "[filter]")
{
    auto v1 = 5;
    auto s = value<char *> { reinterpret_cast<char *>(&v1) };
    auto res = observe(observable::reinterpret_expr_cast<int *>(s));

    REQUIRE(*res.get() == 5);

    auto v2 = 7;
    s = reinterpret_cast<char *>(&v2);

    REQUIRE(*res.get() == 7);
}

}
