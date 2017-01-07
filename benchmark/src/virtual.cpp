#include <chrono>
#include <iostream>
#include <memory>
#include <observable/subject.hpp>
#include "utility.h"

static auto const repeat_count = 1'000'000;
static volatile auto dummy = 0ull;
NOINLINE void function(int v) { dummy += v; }

struct Base
{
    virtual void function(int) =0;
};

struct Derived : Base
{
    virtual void function(int v) { ::function(v); }
};

void bench()
{
    auto base = std::unique_ptr<Base> { std::make_unique<Derived>() };

    auto virtual_duration = benchmark::time_run([&]() { base->function(1); }, repeat_count);

    assert(dummy == repeat_count);
    dummy = 0;

    auto subject = observable::subject<void(int)> { };
    subject.subscribe(function).release();

    auto subject_duration = benchmark::time_run([&]() { subject.notify(1); },
                                                repeat_count);

    assert(dummy == repeat_count);
    dummy = 0;

    benchmark::print("Virtual function", virtual_duration, "Subject", subject_duration);
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
