#include "shared_lib.h"

namespace {
    int count = 0;
}

namespace shared {

void subscribe_in_shared_lib(observable::subject<void()> & subject)
{
    subject.subscribe([&]() {
        ++count;
    }).release();
}

void notify_void(observable::subject<void()> & subject)
{
    subject.notify();
}

int call_count()
{
    return count;
}

void increment_call_count()
{
    ++count;
}

void reset_call_count()
{
    count = 0;
}

}
