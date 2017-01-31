#include <iostream>
#include <string>
#include "observable/observable.hpp"
#include "gtest/gtest.h"
#include "utility.h"

using namespace std::string_literals;

TEST(example_tests, index)
{
    capture_cout cout_buf { };

    // BEGIN EXAMPLE CODE

    using namespace std;
    using namespace observable;

    //int main()
    {
        auto a = value<int> { 5 };
        auto b = value<int> { 7 };
        auto avg = observe((a + b) / 2.0f);

        avg.subscribe([](auto const & val) { cout << val; });

        b = 15;
        // 10 will be printed on stdout.

        //return 0;
    }

    // END EXAMPLE CODE

    ASSERT_EQ("10"s, cout_buf.str());
}
