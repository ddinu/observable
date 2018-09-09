#include <type_traits>
#include <utility>
#include <catch/catch.hpp>
#include <observable/subscription.hpp>

namespace observable { namespace test {

TEST_CASE("infinite_subscription/creation", "[infinite_subscription]")
{
    SECTION("is default-constructible")
    {
        REQUIRE(std::is_default_constructible<infinite_subscription>::value);
    }

    SECTION("can create an initialized subscription")
    {
        infinite_subscription { []() {} };
    }
}

TEST_CASE("infinite_subscription/unsubscribing", "[infinite_subscription]")
{
    SECTION("unsubscribe function is called")
    {
        auto call_count = 0;
        auto sub = infinite_subscription { [&]() { ++call_count; } };

        sub.unsubscribe();

        REQUIRE(call_count == 1);
    }

    SECTION("destructor does not call unsubscribe function")
    {
        auto call_count = 0;

        {
            infinite_subscription { [&]() { ++call_count; } };
        }

        REQUIRE(call_count == 0);
    }

    SECTION("calling unsubscribe() multiple times only calls function once")
    {
        auto call_count = 0;

        auto sub = infinite_subscription { [&]() { ++call_count; } };
        sub.unsubscribe();
        sub.unsubscribe();

        REQUIRE(call_count == 1);
    }

    SECTION("unsubscribing from moved subscription calls function")
    {
        auto call_count = 0;
        infinite_subscription other;

        {
            auto sub = infinite_subscription { [&]() { ++call_count; } };
            other = std::move(sub);
        }

        other.unsubscribe();

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("infinite_subscription/copy", "[infinite_subscription]")
{
    SECTION("is not copy-constructible")
    {
        REQUIRE_FALSE(std::is_copy_constructible<infinite_subscription>::value);
    }

    SECTION("is not copy-assignable")
    {
        REQUIRE_FALSE(std::is_copy_assignable<infinite_subscription>::value);
    }
}

TEST_CASE("infinite_subscription/move", "[infinite_subscription]")
{
    SECTION("is move-constructible")
    {
        REQUIRE(std::is_move_constructible<infinite_subscription>::value);
    }

    SECTION("is move-assignable")
    {
        REQUIRE(std::is_move_assignable<infinite_subscription>::value);
    }
}

} }
