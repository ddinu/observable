#include <chrono>

namespace benchmark {

//! Time how long a number of calls take.
//!
//! \param function The function to time.
//! \param repeat_count Number of times to call the function.
//! \return The average time it takes for a call to return.
template <typename Function>
auto time_run(Function function, unsigned long const repeat_count = 1'000)
{
    auto start = std::chrono::high_resolution_clock::now();

    for(auto i = repeat_count; i > 0; --i)
        function();

    auto end = std::chrono::high_resolution_clock::now();

    return (end - start) / repeat_count;
}

}
