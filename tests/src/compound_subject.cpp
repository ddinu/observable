#include "gtest.h"
#include "observable/compound_subject.hpp"

namespace observable { namespace test {

TEST(compound_subject_test, can_create_subject)
{
    compound_subject<void(), void(int)> { };
}

TEST(compound_subject_test, can_subscribe_with_all_observer_types)
{
    compound_subject<void(), void(int)> s { };
    s.subscribe([&]() { });
    s.subscribe([&](int) { });
}

TEST(compound_subject_test, can_notify_observers)
{
    auto call_count = 0;

    compound_subject<void(), void(int)> s { };
    s.subscribe([&]() { ++call_count; }).release();
    s.subscribe([&](int) { ++call_count; }).release();

    s.notify();
    s.notify(123);

    ASSERT_EQ(call_count, 2);
}

TEST(compound_subject_test, all_compatible_observers_are_called)
{
    auto call_count = 0;

    compound_subject<void(int), void(long)> s { };
    s.subscribe([&](int) { ++call_count; }).release();
    s.subscribe([&](long) { ++call_count; }).release();

    s.notify(123);

    ASSERT_EQ(call_count, 2);
}

TEST(compound_subject_test, incompatible_observers_are_not_called)
{
    auto call_count = 0;

    compound_subject<void(int), void()> s { };
    s.subscribe([&](int) { ++call_count; }).release();
    s.subscribe([&]() { ++call_count; }).release();

    s.notify(123);

    ASSERT_EQ(call_count, 1);
}

TEST(compound_subject_test, can_unsubscribe)
{
    auto call_count = 0;

    compound_subject<void(), void(int)> s { };
    auto sub1 = s.subscribe([&]() { ++call_count; });
    auto sub2 = s.subscribe([&](int) { ++call_count; });

    sub1.unsubscribe();
    sub2.unsubscribe();

    s.notify();
    s.notify(123);

    ASSERT_EQ(call_count, 0);
}

} }
