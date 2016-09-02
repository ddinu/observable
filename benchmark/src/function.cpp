#include <chrono>
#include <iostream>
#include <observable/subject.hpp>
#include "timing.h"

static volatile unsigned long long dummy = 0;
void function() { ++dummy; }

void bench()
{
    auto function_duration = benchmark::time_run(function);

    observable::subject<int> subject;
    subject.subscribe(function);

    auto subject_duration = benchmark::time_run([&]() {
                                subject.notify_untagged();
                            });

    benchmark::print("Function", function_duration, "Subject", subject_duration);
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
