#include <chrono>
#include <iostream>
#include <memory>
#include <observable/subject.hpp>
#include "utility.h"

static volatile unsigned long long dummy = 0;
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
    std::unique_ptr<Base> base = std::make_unique<Derived>();

    auto virtual_duration = benchmark::time_run([&]() { base->function(1); });

    observable::subject<void(int)> subject;
    subject.subscribe(function).release();

    auto subject_duration = benchmark::time_run([&]() { subject.notify(1); });

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
