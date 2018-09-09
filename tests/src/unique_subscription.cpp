#include <type_traits>
#include <utility>
#include <catch/catch.hpp>
#include <observable/subscription.hpp>

namespace observable { namespace test {

TEST_CASE("unique_subscription/creation", "[unique_subscription]")
{
    SECTION("unique subscriptions are default-constructible")
    {
        REQUIRE(std::is_default_constructible<unique_subscription>::value);
    }

    SECTION("can create initialized subscription")
    {
        unique_subscription { infinite_subscription { []() {} } };
    }
}

TEST_CASE("unique_subscription/unsubscribing", "[unique_subscription]")
{
    SECTION("unsubscribe function is called")
    {
        auto call_count = 0;
        auto sub = unique_subscription {
                        infinite_subscription { [&]() { ++call_count; } }
                   };

        sub.unsubscribe();

        REQUIRE(call_count == 1);
    }

    SECTION("destructor calls unsubscribe function")
    {
        auto call_count = 0;

        {
            unique_subscription {
                infinite_subscription { [&]() { ++call_count; } }
            };
        }

        REQUIRE(call_count == 1);
    }

    SECTION("calling unsubscribe multiple times calls function once")
    {
        auto call_count = 0;

        auto sub = unique_subscription {
                        infinite_subscription { [&]() { ++call_count; } }
                   };
        sub.unsubscribe();
        sub.unsubscribe();

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("unique_subscription/copying", "[unique_subscription]")
{
    SECTION("unique subscriptions are not copy-constructible")
    {
        REQUIRE_FALSE(std::is_copy_constructible<unique_subscription>::value);
    }

    SECTION("unique subscriptions are not copy-assignable")
    {
        REQUIRE_FALSE(std::is_copy_assignable<unique_subscription>::value);
    }
}

TEST_CASE("unique_subscription/moving", "[unique_subscription]")
{
    SECTION("unique subscriptions are move-constructible")
    {
        REQUIRE(std::is_move_constructible<unique_subscription>::value);
    }

    SECTION("unique subscriptions are move-assignable")
    {
        REQUIRE(std::is_move_assignable<unique_subscription>::value);
    }

    SECTION("unsubscribing from moved subscription calls function")
    {
        auto call_count = 0;
        unique_subscription other;

        {
            auto sub = unique_subscription {
                            infinite_subscription { [&]() { ++call_count; } }
                       };
            other = std::move(sub);
        }

        other.unsubscribe();

        REQUIRE(call_count == 1);
    }
}

} }
