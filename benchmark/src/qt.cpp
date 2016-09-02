#include "qt.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <observable/subject.hpp>
#include "timing.h"

static volatile unsigned long long dummy = 0;
void function() { ++dummy; }

void bench()
{
    Sender sender;
    Receiver receiver;

    auto success = receiver.connect(&sender, &Sender::inc,
                                    &receiver, &Receiver::inc,
                                    Qt::DirectConnection);
    (void)success;
    assert(success);

    auto qt_duration = benchmark::time_run([&]() {
                            emit sender.inc();
                       });

    observable::subject<int> subject;
    subject.subscribe([]() { function(); });

    auto subject_duration = benchmark::time_run([&]() {
                                subject.notify_untagged();
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
