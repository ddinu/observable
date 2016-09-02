#include <iostream>
#include <chrono>
#include <observable/subject.hpp>
#include "timing.h"

static volatile unsigned long long dummy = 0;
void function() { ++dummy; }

int main()
{
    auto function_duration = benchmark::time_run(function);

    observable::subject<int> subject;
    subject.subscribe(function);

    auto subject_duration = benchmark::time_run([&]() {
                                subject.notify_untagged();
                            });

    std::cout << "Function run duration: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(function_duration).count()
              << "ns\nSubject duration: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(subject_duration).count()
              << "ns\n" << subject_duration / function_duration << " times slower."
              << std::endl;

    return 0;
}
