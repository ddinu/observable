#include "gtest.h"
#include "observable/subject.hpp"
#include "src/shared_lib.h"

namespace observable { namespace test {

TEST(shared_lib_test, can_subscribe_to_subject_from_inside_shared_lib)
{
    subject<void()> s { };

    shared::reset_call_count();
    shared::subscribe_in_shared_lib(s);
    s.notify();

    ASSERT_EQ(shared::call_count(), 1);
}

TEST(shared_lib_test, can_notify_subject_from_inside_shared_lib)
{
    subject<void()> s { };
    int call_count = 0;

    s.subscribe([&]() { ++call_count; }).release();
    shared::notify_void(s);

    ASSERT_EQ(call_count, 1);
}

} }
