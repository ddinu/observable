#include <iostream>
#include "observable/observable.hpp"
#include "gtest/gtest.h"
#include "utility.h"

using namespace std::string_literals;

TEST(example_tests, readme)
{
    provide_cin cin_buf { "Jane"s };
    capture_cout cout_buf { };

    // BEGIN EXAMPLE CODE

    using namespace std;
    using namespace observable;

    //int main()
    {
        auto sub = subject<void(string)> { };
        sub.subscribe([](auto const & str) { cout << str << endl; });

        // "Hello world!" will be printed on stdout.
        sub.notify("Hello world!");

        auto a = value<int> { 5 };
        auto b = value<int> { 7 };
        auto avg = observe((a + b) / 2.0f);

        avg.subscribe([](auto val) { cout << val << endl; });

        // 10 will be printed on stdout.
        b = 15;

        //return 0;
    }

    // END EXAMPLE CODE

    ASSERT_EQ("Hello world!\n10\n"s, cout_buf.str());
}
