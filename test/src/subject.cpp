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
using tsubject = subject<std::string>;

TEST(subject_test, can_create_subject)
{
    tsubject { };
}

TEST(subject_test, can_subscribe_to_untagged_changes)
{
    tsubject s;
    s.subscribe(dummy);
    s.subscribe([=]() { });
    s.subscribe([=]() mutable { });
    s.subscribe(dummy_args);
    s.subscribe([=](int, float) { });
    s.subscribe([=](int, float) mutable { });
}

TEST(subject_test, can_subscribe_to_tagged_changes)
{
    tsubject s;
    s.subscribe("tagged", dummy);
    s.subscribe("tagged", [=]() { });
    s.subscribe("tagged", [=]() mutable { });
    s.subscribe("tagged", dummy_args);
    s.subscribe("tagged", [=](int, float) { });
    s.subscribe("tagged", [=](int, float) mutable { });
}

TEST(subject_test, untagged_observers_are_called)
{
    tsubject s;

    auto call_count = 0;
    s.subscribe([&]() { ++call_count; });
    s.subscribe([&]() { ++call_count; });
    s.notify_untagged();

    ASSERT_EQ(call_count, 2);
}

TEST(subject_test, tagged_observers_are_called)
{
    tsubject s;

    auto call_count = 0;
    s.subscribe("tag", [&]() { ++call_count; });
    s.subscribe("tag", [&]() { ++call_count; });
    s.notify_tagged("tag");

    ASSERT_EQ(call_count, 2);
}

TEST(subject_test, observer_with_const_reference_parameters_is_called)
{
    tsubject s;
    auto call_count = 0;

    s.subscribe([&](int const &) { ++call_count; });
    s.notify_untagged(5);

    ASSERT_EQ(call_count, 1);
}

TEST(subject_test, notify_with_const_reference_parameter_calls_observer)
{
    tsubject s;
    auto call_count = 0;

    s.subscribe([&](int) { ++call_count; });
    s.notify_untagged<int const &>(5);

    ASSERT_EQ(call_count, 1);
}

TEST(subject_test, observer_is_not_called_after_unsubscribing)
{
    tsubject s;
    auto call_count = 0;

    auto sub = s.subscribe([&]() { ++call_count; });
    sub.unsubscribe();
    s.notify_untagged();

    ASSERT_EQ(call_count, 0);
}

TEST(subject_test, is_movable)
{
    ASSERT_TRUE(std::is_move_constructible<subject<>>::value);
    ASSERT_TRUE(std::is_move_assignable<subject<>>::value);
}

TEST(subject_test, is_not_copyable)
{
    ASSERT_FALSE((std::is_copy_constructible<subject<>>::value));
    ASSERT_FALSE((std::is_copy_assignable<subject<>>::value));
}

TEST(subject_test, moved_subject_works)
{
    tsubject s1;
    auto call_count = 0;

    s1.subscribe([&]() { ++call_count; });
    auto s2 = std::move(s1);
    s2.notify_untagged();

    ASSERT_EQ(call_count, 1);
}

TEST(subject_test, can_use_auto_unsubscribe)
{
    tsubject s;
    auto_unsubscribe sub = s.subscribe([]() {});

    ASSERT_TRUE(!!sub);
}

TEST(subject_test, observer_added_from_another_observer_is_called_on_second_notification)
{
    tsubject s;
    auto call_count = 0;

    auto sub = s.subscribe([&]() {
        ++call_count;
        s.subscribe([&]() { ++call_count; });
    });

    s.notify_untagged();
    s.notify_untagged();

    ASSERT_EQ(call_count, 3);
}

TEST(subject_test, observers_run_on_the_thread_that_calls_notify)
{
    tsubject s;
    std::thread::id other_id;

    s.subscribe([&]() { other_id = std::this_thread::get_id(); });
    std::thread { [&]() { s.notify_untagged(); } }.join();

    ASSERT_NE(other_id, std::this_thread::get_id());
}

TEST(subject_test, observer_added_from_other_thread_while_notification_is_running_is_not_called)
{
    tsubject s;
    std::atomic_int old_call_count { 0 };
    std::atomic_int new_call_count { 0 };

    for(auto i = 0; i < 10; ++i)
        s.subscribe([&]() { ++old_call_count; std::this_thread::sleep_for(5ms); });

    std::thread t { [&]() { s.notify_untagged(); } };

    for(auto i = 0; i < 100 && old_call_count == 0; ++i)
        std::this_thread::sleep_for(1ms);

    for(auto i = 0; i < 10; ++i)
        s.subscribe([&]() { ++new_call_count; });

    t.join();

    ASSERT_EQ(old_call_count, 10);
    ASSERT_EQ(new_call_count, 0);
}

TEST(subject_test, can_unsubscribe_while_notification_is_running)
{
    tsubject s;
    std::atomic_int call_count { 0 };
    std::vector<auto_unsubscribe> subs;

    for(auto i = 0; i < 10; ++i)
        subs.push_back(
                s.subscribe([&]() {
                    ++call_count;
                    std::this_thread::sleep_for(5ms);
                }));

    std::thread t { [&]() { s.notify_untagged(); } };

    for(auto i = 0; i < 100 && call_count == 0; ++i)
        std::this_thread::sleep_for(1ms);

    subs.clear();
    t.join();

    ASSERT_EQ(call_count, 10);
}

} }
