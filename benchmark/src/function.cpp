#include <chrono>
#include <iostream>
#include <observable/subject.hpp>
#include "utility.h"

static volatile unsigned long long dummy = 0;
NOINLINE void function1() { ++dummy; }
NOINLINE void function2(int v) { dummy += v; }
NOINLINE void function3(int v1, int v2, int v3, double v4) { dummy += v1 + v2 + v3 + (int)v4; }

void bench()
{
    auto function_duration = benchmark::time_run(function1);
    function_duration += benchmark::time_run([]() { function2(1); });
    function_duration += benchmark::time_run([]() { function3(1, 2, 3, 4); });

    observable::subject<int> subject;
    subject.subscribe(function1);
    subject.subscribe(function2);
    subject.subscribe(function3);

    auto subject_duration = benchmark::time_run([&]() {
                                subject.notify_untagged();
                            });
    subject_duration += benchmark::time_run([&]() {
                            subject.notify_untagged(1);
                        });
    subject_duration += benchmark::time_run([&]() {
                            subject.notify_untagged(1, 2, 3, 4.0);
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
