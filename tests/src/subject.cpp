#include <atomic>
#include <string>
#include <utility>
#include <thread>
#include <type_traits>
#include <chrono>
#include <vector>
#include "gtest.h"
#include "observable/subject.hpp"

namespace observable { namespace test {

void dummy() { }
void dummy_args(int, float) { }

using namespace std::chrono_literals;

TEST(subject_test, is_default_constructible)
{
    subject<void()> { };

    ASSERT_TRUE(std::is_default_constructible<subject<void()>>::value);
}

TEST(subject_test, can_subscribe_to_subject)
{
    subject<void()> s1;
    s1.subscribe(dummy);
    s1.subscribe([=]() { });
    s1.subscribe([=]() mutable { });

    subject<void(int, float)> s2;
    s2.subscribe(dummy_args);
    s2.subscribe([=](int, float) { });
    s2.subscribe([=](int, float) mutable { });
    s2.subscribe([=](auto, auto) { });
    s2.subscribe([=](auto, auto) mutable { });
}

TEST(subject_test, observers_are_called)
{
    subject<void()> s;

    auto call_count = 0;
    s.subscribe([&]() { ++call_count; }).release();
    s.subscribe([&]() { ++call_count; }).release();
    s.notify();

    ASSERT_EQ(call_count, 2);
}

TEST(subject_test, observer_with_const_reference_parameters_is_called)
{
    subject<void(int)> s;
    auto call_count = 0;

    s.subscribe([&](int const &) { ++call_count; }).release();
    s.notify(5);

    ASSERT_EQ(call_count, 1);
}

TEST(subject_test, subject_with_const_reference_parameter_calls_observer)
{
    subject<void(int const &)> s;
    auto call_count = 0;

    s.subscribe([&](int) { ++call_count; }).release();
    s.notify(5);

    ASSERT_EQ(call_count, 1);
}

TEST(subject_test, observer_is_not_called_after_unsubscribing)
{
    subject<void()> s;
    auto call_count = 0;

    auto sub = s.subscribe([&]() { ++call_count; });
    sub.unsubscribe();
    s.notify();

    ASSERT_EQ(call_count, 0);
}

TEST(subject_test, is_nothrow_move_constructible)
{
    ASSERT_TRUE(std::is_nothrow_move_constructible<subject<void()>>::value);
}

TEST(subject_test, is_nothrow_move_assignable)
{
    ASSERT_TRUE(std::is_nothrow_move_assignable<subject<void()>>::value);
}

TEST(subject_test, is_not_copy_constructible)
{
    ASSERT_FALSE(std::is_copy_constructible<subject<void()>>::value);
}

TEST(subject_test, is_not_copy_assignable)
{
    ASSERT_FALSE(std::is_copy_assignable<subject<void()>>::value);
}

TEST(subject_test, moved_subject_keeps_subscribed_observers)
{
    subject<void()> s1;
    auto call_count = 0;

    s1.subscribe([&]() { ++call_count; }).release();
    auto s2 = std::move(s1);
    s2.notify();

    ASSERT_EQ(call_count, 1);
}

TEST(subject_test, observer_added_from_running_notify_is_called_on_second_notification)
{
    subject<void()> s;
    auto call_count = 0;

    auto sub = s.subscribe([&]() {
        ++call_count;
        s.subscribe([&]() { ++call_count; }).release();
    });

    s.notify();
    ASSERT_EQ(call_count, 1);

    s.notify();
    ASSERT_EQ(call_count, 3);
}

TEST(subject_test, observers_run_on_the_thread_that_calls_notify)
{
    subject<void()> s;
    std::thread::id other_id;

    s.subscribe([&]() { other_id = std::this_thread::get_id(); });
    std::thread { [&]() { s.notify(); } }.join();

    ASSERT_NE(other_id, std::this_thread::get_id());
}

TEST(subject_test, observer_added_from_other_thread_while_notification_is_running_is_not_called)
{
    subject<void()> s;
    std::atomic_int old_call_count { 0 };
    std::atomic_int new_call_count { 0 };

    for(auto i = 0; i < 10; ++i)
        s.subscribe([&]() { ++old_call_count; std::this_thread::sleep_for(5ms); }).release();

    std::thread t { [&]() { s.notify(); } };

    for(auto i = 0; i < 100 && old_call_count == 0; ++i)
        std::this_thread::sleep_for(1ms);

    for(auto i = 0; i < 10; ++i)
        s.subscribe([&]() { ++new_call_count; }).release();

    t.join();

    ASSERT_EQ(old_call_count, 10);
    ASSERT_EQ(new_call_count, 0);
}

TEST(subject_test, can_unsubscribe_while_notification_is_running)
{
    subject<void()> s;
    std::atomic_int call_count { 0 };
    std::vector<shared_subscription> subs;

    for(auto i = 0; i < 10; ++i)
        subs.emplace_back(
                s.subscribe([&]() {
                    ++call_count;
                    std::this_thread::sleep_for(5ms);
                }));

    std::thread t { [&]() { s.notify(); } };

    for(auto i = 0; i < 100 && call_count == 0; ++i)
        std::this_thread::sleep_for(1ms);

    subs.clear();
    t.join();

    ASSERT_EQ(1, call_count);
}

} }
