#include <chrono>
#include <iostream>
#include <memory>
#include <observable/observable.hpp>
#include "utility.h"

static auto const repeat_count = 100'000;
static auto const loop_count = 4;
volatile int dummy = 0;

NOINLINE void consume(int v) { dummy += v; }

void bench()
{
    auto duration = benchmark::time_run([&]() {
        for(int i = 0; i < loop_count; ++i)
            for(int j = 0; j < loop_count; ++j)
                for(int k = 0; k < loop_count; ++k)
                    consume(500 + (i * j + k) - (1 + k + j));
    }, repeat_count);

    std::chrono::nanoseconds expr_duration;
    {
        auto i = observable::value<int> { };
        auto j = observable::value<int> { };
        auto k = observable::value<int> { };

        auto result = observable::observe(500 + (i * j * k) - (1 + k + j));
        result.subscribe([](auto && x) { consume(x); }).release();

        expr_duration = benchmark::time_run([&]() {
            for(i = 0; i.get() < loop_count; i = i.get() + 1)
                for(j = 0; j.get() < loop_count; j = j.get() + 1)
                    for(k = 0; k.get() < loop_count; k = k.get() + 1)
                        ;
        }, repeat_count);
    }

    benchmark::print("Normal expression", duration, "Observable expression", expr_duration);
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
