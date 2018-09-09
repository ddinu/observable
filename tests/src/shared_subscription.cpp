#include <memory>
#include <type_traits>
#include <catch/catch.hpp>
#include <observable/subscription.hpp>

namespace observable { namespace test {

TEST_CASE("shared_subscripton/creation", "[shared_subscription]")
{
    SECTION("shared subscriptions are default-constructible")
    {
        REQUIRE(std::is_nothrow_default_constructible<shared_subscription>::value);
    }

    SECTION("can create a shared_subscription from an infinite_subscription")
    {
        shared_subscription { infinite_subscription { []() {} } };
    }

    SECTION("can create a shared_subscription from an unique_subscription")
    {
        shared_subscription {
            unique_subscription {
                infinite_subscription { []() {} }
            }
        };
    }
}

TEST_CASE("shared_subscripton/unsubscribing", "[shared_subscription]")
{
    SECTION("unsubscribe is called when destroyed")
    {
        auto call_count = 0;

        {
            shared_subscription { infinite_subscription { [&]() { ++call_count; } } };
        }

        REQUIRE(call_count == 1);
    }

    SECTION("can manually call unsubscribe()")
    {
        auto call_count = 0;

        auto sub = shared_subscription { infinite_subscription { [&]() { ++call_count; } } };
        sub.unsubscribe();

        REQUIRE(call_count == 1);
    }

    SECTION("unsubscribe is called by last instance when destroyed")
    {
        auto call_count = 0;

        {
            auto sub = shared_subscription {
                infinite_subscription { [&]() { ++call_count; } }
            };

            {
                auto copy = sub;
            }

            REQUIRE(call_count == 0);
        }

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("shared_subscripton/copying", "[shared_subscription]")
{
    SECTION("shared subscriptions are copy-constructible")
    {
        REQUIRE(std::is_copy_constructible<shared_subscription>::value);
    }

    SECTION("shared subscriptions are copy-assignable")
    {
        REQUIRE(std::is_copy_assignable<shared_subscription>::value);
    }
}

TEST_CASE("shared_subscripton/moving", "[shared_subscripton]")
{
    SECTION("shared subscriptions are move-constructible")
    {
        REQUIRE(std::is_move_constructible<shared_subscription>::value);
    }

    SECTION("shared subscriptions are move-assignable")
    {
        REQUIRE(std::is_move_assignable<shared_subscription>::value);
    }
}

} }
