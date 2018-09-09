#include <cmath>
#include <catch/catch.hpp>
#include <observable/observe.hpp>
#include <observable/value.hpp>
#include <observable/expressions/math.hpp>

namespace { // Filters should be usable with ADL.

using observable::value;
using namespace Catch::literals;

TEST_CASE("math filter/abs", "[math filter]")
{
    auto a = value<int> { 5 };
    auto res = observe(abs(a));

    REQUIRE(res.get() == 5);

    a = -7;
    REQUIRE(res.get() == 7);
}

TEST_CASE("math filter/div", "[make filter]")
{
    auto x = value<int> { 5 };
    auto y = value<int> { 3 };
    auto res = observe(div(x, y));

    REQUIRE(res.get().quot == std::div(5, 3).quot);
    REQUIRE(res.get().rem == std::div(5, 3).rem);

    x = 31;
    REQUIRE(res.get().quot == std::div(31, 3).quot);
    REQUIRE(res.get().rem == std::div(31, 3).rem);
}

TEST_CASE("math filter/exp", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(exp(a));

    REQUIRE(res.get() == Approx { std::exp(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::exp(7.0) });
}

TEST_CASE("math filter/exp2", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(exp2(a));

    REQUIRE(res.get() == Approx { std::exp2(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::exp2(7.0) });
}

TEST_CASE("math filter/log", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(log(a));

    REQUIRE(res.get() == Approx { std::log(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::log(7.0) });
}

TEST_CASE("math filter/log10", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(log10(a));

    REQUIRE(res.get() == Approx { std::log10(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::log10(7.0) });
}

TEST_CASE("math filter/log2", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(log2(a));

    REQUIRE(res.get() == Approx { std::log2(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::log2(7.0) });
}

TEST_CASE("math filter/pow", "[math filter]")
{
    auto b = value<double> { 5 };
    auto e = value<double> { 3 };
    auto res = observe(pow(b, e));

    REQUIRE(res.get() == Approx { std::pow(5, 3) });

    b = 7;
    REQUIRE(res.get() == Approx { std::pow(7, 3) });

    e = 4;
    REQUIRE(res.get() == Approx { std::pow(7, 4) });
}

TEST_CASE("math filter/sqrt", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(sqrt(a));

    REQUIRE(res.get() == Approx { std::sqrt(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::sqrt(7.0) });
}

TEST_CASE("math filter/cbrt", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(cbrt(a));

    REQUIRE(res.get() == Approx { std::cbrt(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::cbrt(7.0) });
}

TEST_CASE("math filter/hypot", "[math filter]")
{
    auto x = value<double> { 5 };
    auto y = value<double> { 7 };
    auto res = observe(hypot(x, y));

    REQUIRE(res.get() == Approx { std::hypot(5, 7) });

    x = 10;
    REQUIRE(res.get() == Approx { std::hypot(10, 7) });

    y = 20;
    REQUIRE(res.get() == Approx { std::hypot(10, 20) });
}

TEST_CASE("math filter/sin", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(sin(a));

    REQUIRE(res.get() == Approx { std::sin(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::sin(7.0) });
}

TEST_CASE("math filter/cos", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(cos(a));

    REQUIRE(res.get() == Approx { std::cos(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::cos(7.0) });
}

TEST_CASE("math filter/tan", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(tan(a));

    REQUIRE(res.get() == Approx { std::tan(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::tan(7.0) });
}

TEST_CASE("math filter/asin", "[math filter]")
{
    auto a = value<double> { 0.5 };
    auto res = observe(asin(a));

    REQUIRE(res.get() == Approx { std::asin(0.5) });

    a = 0.8;
    REQUIRE(res.get() == Approx { std::asin(0.8) });
}

TEST_CASE("math filter/acos", "[math filter]")
{
    auto a = value<double> { 0.5 };
    auto res = observe(acos(a));

    REQUIRE(res.get() == Approx { std::acos(0.5) });

    a = 0.8;
    REQUIRE(res.get() == Approx { std::acos(0.8) });
}

TEST_CASE("math filter/atan", "[math filter]")
{
    auto a = value<double> { 5 };
    auto res = observe(atan(a));

    REQUIRE(res.get() == Approx { std::atan(5.0) });

    a = 7;
    REQUIRE(res.get() == Approx { std::atan(7.0) });
}

TEST_CASE("math filter/atan2", "[math filter]")
{
    auto y = value<double> { 5 };
    auto x = value<double> { 7 };
    auto res = observe(atan2(y, x));

    REQUIRE(res.get() == Approx { std::atan2(5, 7) });

    y = 17;
    REQUIRE(res.get() == Approx { std::atan2(17, 7) });

    x = 10;
    REQUIRE(res.get() == Approx { std::atan2(17, 10) });
}

TEST_CASE("math filter/ceil", "[math filter]")
{
    auto a = value<double> { 0.4 };
    auto res = observe(ceil(a));

    REQUIRE(res.get() == 1_a);

    a = 1.8;
    REQUIRE(res.get() == 2_a);
}

TEST_CASE("math filter/floor", "[math filter]")
{
    auto a = value<double> { 0.7 };
    auto res = observe(floor(a));

    REQUIRE(res.get() == 0_a);

    a = 1.8;
    REQUIRE(res.get() == 1_a);
}

TEST_CASE("math filter/trunc", "[math filter]")
{
    auto a = value<double> { 0.7 };
    auto res = observe(trunc(a));

    REQUIRE(res.get() == 0_a);

    a = 1.8;
    REQUIRE(res.get() == 1_a);
}

TEST_CASE("math filter/round", "[math filter]")
{
    auto a = value<double> { 0.4 };
    auto res = observe(round(a));

    REQUIRE(res.get() == 0_a);

    a = 1.8;
    REQUIRE(res.get() == 2_a);
}

}
