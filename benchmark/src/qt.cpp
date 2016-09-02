#include "qt.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <observable/subject.hpp>
#include "timing.h"

static volatile unsigned long long dummy = 0;
void function() { ++dummy; }

int main()
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

    std::cout << "Qt signal-slot run duration: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(qt_duration).count()
              << "ns\nSubject duration: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(subject_duration).count()
              << "ns\n" << subject_duration / qt_duration << " times slower."
              << std::endl;

    return 0;
}
