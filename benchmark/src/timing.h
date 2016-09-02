#include <algorithm>
#include <chrono>
#include <string>
#include <iostream>

namespace benchmark {

//! Time how long a number of calls take.
//!
//! \param function The function to time.
//! \param repeat_count Number of times to call the function.
//! \return The average time it takes for a call to return.
template <typename Function>
std::chrono::nanoseconds
time_run(Function function, unsigned long const repeat_count = 1'000'000)
{
    auto start = std::chrono::high_resolution_clock::now();

    for(auto i = repeat_count; i > 0; --i)
        function();

    auto end = std::chrono::high_resolution_clock::now();

    return (end - start) / repeat_count;
}

//! Compute how much slower ``slow`` is compared to ``fast``. If ``slow`` is
//! actually faster than ``fast``, the result will be negative.
double slowdown(std::chrono::nanoseconds fast, std::chrono::nanoseconds slow)
{
    auto f = static_cast<long double>(fast.count());
    auto s = static_cast<long double>(slow.count());

    return (std::max(f, s) - std::min(f, s)) / std::min(f, s) * (f < s ? 1 : -1);
}

//! Print timings.
void print(std::string first_name, std::chrono::nanoseconds first_duration,
           std::string second_name, std::chrono::nanoseconds second_duration)
{
    std::cout << first_name << " run duration: " << first_duration.count() << "ns\n"
              << second_name << " run duration: " << second_duration.count() << "ns\n";

    if(first_duration < second_duration)
        std::cout << second_name
                  << " slowdown: "
                  << slowdown(first_duration, second_duration);
    else
        std::cout << second_name
                  << " speedup: "
                  << slowdown(second_duration, first_duration);

    std::cout << std::endl;
}

}
