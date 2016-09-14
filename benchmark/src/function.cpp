#include <chrono>
#include <iostream>
#include <observable/subject.hpp>
#include "utility.h"

static volatile unsigned long long dummy = 0;
NOINLINE void function(int v) { dummy += v; }

void bench()
{
    auto function_duration = benchmark::time_run([]() { function(1); });

    observable::subject<void(int)> subject;
    subject.subscribe(function).release();

    auto subject_duration = benchmark::time_run([&]() { subject.notify(1); });

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
