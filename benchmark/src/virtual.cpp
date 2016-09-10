#include <chrono>
#include <iostream>
#include <memory>
#include <observable/subject.hpp>
#include "utility.h"

static volatile unsigned long long dummy = 0;
NOINLINE void function1() { ++dummy; }
NOINLINE void function2(int v) { dummy += v; }
NOINLINE void function3(int v1, int v2, int v3, double v4) { dummy += v1 + v2 + v3 + (int)v4; }

struct Base
{
    virtual void function1() =0;
    virtual void function2(int) =0;
    virtual void function3(int, int, int, double) =0;
};

struct Derived : Base
{
    virtual void function1() { ::function1(); }
    virtual void function2(int v) { ::function2(v); }
    virtual void function3(int v1, int v2, int v3, double v4) { ::function3(v1, v2, v3, v4); }
};

void bench()
{
    std::unique_ptr<Base> base = std::make_unique<Derived>();

    auto virtual_duration = benchmark::time_run([&]() { base->function1(); });
    virtual_duration += benchmark::time_run([&]() { base->function2(1); });
    virtual_duration += benchmark::time_run([&]() { base->function3(1, 2, 3, 4); });

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
