#include <atomic>
#include <string>
#include <utility>
#include <thread>
#include <type_traits>
#include <chrono>
#include <vector>
#include <catch/catch.hpp>
#include <observable/subject.hpp>

namespace observable { namespace test {

void dummy() { }
void dummy_args(int, float) { }

using namespace std::chrono_literals;

TEST_CASE("subject/subjects are default-constructible", "[subject]")
{
    subject<void()> { };
    REQUIRE(std::is_default_constructible<subject<void()>>::value);
}

TEST_CASE("subject/can subscribe to subject", "[subject]")
{
    auto s1 = subject<void()> { };
    s1.subscribe(dummy);
    s1.subscribe([=]() { });
    s1.subscribe([=]() mutable { });

    auto s2 = subject<void(int, float)> { };
    s2.subscribe(dummy_args);
    s2.subscribe([=](int, float) { });
    s2.subscribe([=](int, float) mutable { });
    s2.subscribe([=](auto, auto) { });
    s2.subscribe([=](auto, auto) mutable { });
}

TEST_CASE("subject/can notify subject with no subscribed observers", "[subject]")
{
    auto s = subject<void()> { };
    s.notify();
}

TEST_CASE("subject/observers are called", "[subject]")
{
    SECTION("observers are called")
    {
        auto s = subject<void()> { };

        auto call_count = 0;
        s.subscribe([&]() { ++call_count; }).release();
        s.subscribe([&]() { ++call_count; }).release();
        s.notify();

        REQUIRE(call_count == 2);
    }

    SECTION("observer with const reference parameters is called")
    {
        auto s = subject<void(int)> { };
        auto call_count = 0;

        s.subscribe([&](int const &) { ++call_count; }).release();
        s.notify(5);

        REQUIRE(call_count == 1);
    }

    SECTION("subject with const reference parameter calls observer")
    {
        auto s = subject<void(int const &)> { };
        auto call_count = 0;

        s.subscribe([&](int) { ++call_count; }).release();
        s.notify(5);

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("subject/observer is not called after unsubscribing", "[subject]")
{
    auto s = subject<void()> { };
    auto call_count = 0;

    auto sub = s.subscribe([&]() { ++call_count; });
    sub.unsubscribe();
    s.notify();

    REQUIRE(call_count == 0);
}

TEST_CASE("subject/copying", "[subject]")
{
    SECTION("subjects are not copy-constructible")
    {
        REQUIRE_FALSE(std::is_copy_constructible<subject<void()>>::value);
    }

    SECTION("subjects are not copy-assignable")
    {
        REQUIRE_FALSE(std::is_copy_assignable<subject<void()>>::value);
    }
}

TEST_CASE("subject/moving", "[subject]")
{
    SECTION("subjects are nothrow move-constructible")
    {
        REQUIRE(std::is_nothrow_move_constructible<subject<void()>>::value);
    }

    SECTION("subjects are nothrow move-assignable")
    {
        REQUIRE(std::is_nothrow_move_assignable<subject<void()>>::value);
    }

    SECTION("moved subject keeps subscribed observers")
    {
        auto s1 = subject<void()> { };
        auto call_count = 0;

        s1.subscribe([&]() { ++call_count; }).release();
        auto s2 = std::move(s1);
        s2.notify();

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("subject/subjects are reentrant", "[subject]")
{
    SECTION("observer added from running notify is called on second notification")
    {
        auto s = subject<void()> { };
        auto call_count = 0;

        auto sub = s.subscribe([&]() {
            ++call_count;
            s.subscribe([&]() { ++call_count; }).release();
        });

        s.notify();
        REQUIRE(call_count == 1);

        s.notify();
        REQUIRE(call_count == 3);
    }

    SECTION("can unsubscribe while notification is running")
    {
        auto s = subject<void()> { };
        auto call_count = 0;

        auto sub = unique_subscription { };
        sub = s.subscribe([&]() { ++call_count; sub.unsubscribe(); });

        s.notify();
        s.notify();

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("subject/observers run on the thread that calls notify", "[subject]")
{
    auto s = subject<void()> { };
    auto other_id = std::thread::id { };

    s.subscribe([&]() { other_id = std::this_thread::get_id(); });
    std::thread { [&]() { s.notify(); } }.join();

    REQUIRE(std::this_thread::get_id() != other_id);
}

TEST_CASE("subject/observer added during notification is not called", "[subject]")
{
    auto s = subject<void()> { };
    std::atomic_int old_call_count { 0 };
    std::atomic_int new_call_count { 0 };

    for(auto i = 0; i < 10; ++i)
        s.subscribe([&]() {
            ++old_call_count;
            std::this_thread::sleep_for(5ms);
        }).release();

    auto t = std::thread { [&]() { s.notify(); } };

    for(auto i = 0; i < 100 && old_call_count == 0; ++i)
        std::this_thread::sleep_for(1ms);

    for(auto i = 0; i < 10; ++i)
        s.subscribe([&]() { ++new_call_count; }).release();

    t.join();

    REQUIRE(old_call_count == 10);
    REQUIRE(new_call_count == 0);
}

TEST_CASE("subject/can use subject enclosed in class", "[subject]")
{
    struct Foo
    {
        subject<void(), Foo> test;

        void notify_test() { test.notify(); }
    } foo;

    auto called = false;
    foo.test.subscribe([&]() { called = true; }).release();
    foo.notify_test();

    REQUIRE(called);
}

TEST_CASE("subject/can subscribe and immediately call observer", "[subject]")
{
    auto s = subject<void()> { };

    auto call_count = 0;
    auto sub = s.subscribe_and_call([&]() { ++call_count; });

    REQUIRE(call_count == 1);
}

TEST_CASE("subject/immediately called observer receives arguments", "[subject]")
{
    auto s = subject<void(int)> { };

    auto arg = 0;
    auto sub = s.subscribe_and_call([&](int v) { arg = v; }, 7);

    REQUIRE(arg == 7);
}

TEST_CASE("subject/empty", "[subject]")
{
    SECTION("empty returns true for subject with no subscribers")
    {
        auto const s = subject<void()> { };

        REQUIRE(s.empty());
    }

    SECTION("empty returns false for subject with subscribers")
    {
        auto s = subject<void()> { };
        auto const sub = s.subscribe([]() { });

        REQUIRE_FALSE(s.empty());
    }
}

} }
