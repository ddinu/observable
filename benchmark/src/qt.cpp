#include "qt.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <observable/subject.hpp>

static auto const repeat_count = 1'000'000;
static auto const sub_count = 10;

void bench()
{
    Sender sender;
    Receiver receiver;

    for(auto i = 0; i < sub_count; ++i)
    {
        auto success = receiver.connect(&sender, &Sender::inc,
                                        &receiver, &Receiver::inc,
                                        Qt::DirectConnection);
        (void)success;
        assert(success);
    }

    auto qt_duration = benchmark::time_run([&]() { emit sender.inc(1); }, repeat_count);

    assert(receiver.dummy == repeat_count * sub_count);
    receiver.dummy = 0;

    auto subject = observable::subject<void(int)> { };

    for(auto i = 0; i < sub_count; ++i)
        subject.subscribe([&](int v) { receiver.inc(v); }).release();

    auto subject_duration = benchmark::time_run([&]() { subject.notify(1); }, repeat_count);

    assert(receiver.dummy == repeat_count * sub_count);
    receiver.dummy = 0;

    benchmark::print("Qt signal-slot", qt_duration, "Subject", subject_duration);
}

int main()
{
    for(auto i = 0; i < 5; ++i)
    {
        bench();
        std::cout << std::endl;
    }

    return 0;
}
