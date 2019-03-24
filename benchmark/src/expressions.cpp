#include <chrono>
#include <iostream>
#include <memory>
#include <observable/observable.hpp>
#include "utility.h"

static auto const repeat_count = 100'000;
volatile int dummy = 0;

NOINLINE void consume(int v) { dummy += v; }

void bench()
{
    auto duration = benchmark::time_run([&]() {
        for(int i = 0; i < 5; ++i)
            for(int j = 0; j < 5; ++j)
                for(int k = 0; k < 5; ++k)
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
            for(i = 0; i.get() < 5; i = i.get() + 1)
                for(j = 0; j.get() < 5; j = j.get() + 1)
                    for(k = 0; k.get() < 5; k = k.get() + 1)
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
