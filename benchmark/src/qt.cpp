#include "qt.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <observable/subject.hpp>

void bench()
{
    Sender sender;
    Receiver receiver;

    auto success = receiver.connect(&sender, &Sender::inc1,
                                    &receiver, &Receiver::inc1,
                                    Qt::DirectConnection);
    assert(success);

    success = receiver.connect(&sender, &Sender::inc2,
                               &receiver, &Receiver::inc2,
                               Qt::DirectConnection);
    assert(success);

    success = receiver.connect(&sender, &Sender::inc3,
                               &receiver, &Receiver::inc3,
                               Qt::DirectConnection);
    assert(success);

    auto qt_duration = benchmark::time_run([&]() {
                            emit sender.inc1();
                            emit sender.inc2(1);
                            emit sender.inc3(1, 2, 3, 4);
                       });

    observable::subject<int> subject;
    subject.subscribe([&]() { receiver.inc1(); });
    subject.subscribe([&](int v) { receiver.inc2(v); });
    subject.subscribe([&](int v1, int v2, int v3, double v4) { receiver.inc3(v1, v2, v3, v4); });

    auto subject_duration = benchmark::time_run([&]() {
                                subject.notify_untagged();
                                subject.notify_untagged(1);
                                subject.notify_untagged(1, 2, 3, 4.0);
                            });

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
