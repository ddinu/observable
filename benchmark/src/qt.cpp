#include "qt.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <observable/subject.hpp>

void bench()
{
    Sender sender;
    Receiver receiver;

    auto success = receiver.connect(&sender, &Sender::inc,
                                    &receiver, &Receiver::inc,
                                    Qt::DirectConnection);
    (void)success;
    assert(success);

    auto qt_duration = benchmark::time_run([&]() { emit sender.inc(1); });

    observable::subject<void(int)> subject;
    subject.subscribe([&](int v) { receiver.inc(v); }).release();

    auto subject_duration = benchmark::time_run([&]() { subject.notify(1); });

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
