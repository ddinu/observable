#include <iostream>
#include <observable/observable.hpp>
#include "gtest.h"
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
        sub.subscribe([](auto const & msg) { cout << msg << endl; });

        // "Hello world!" will be printed on stdout.
        sub.notify("Hello world!");

        auto a = value<int> { 5 };
        auto b = value<int> { 5 };
        auto avg = observe(
                        (a + b) / 2.0f
                   );
        auto eq_msg = observe(
                        select(a == b, "equal", "not equal")
                       );

        avg.subscribe([](auto val) { cout << val << endl; });
        eq_msg.subscribe([](auto const & msg) { cout << msg << endl; });

        // "10" and "not equal" will be printed on stdout in an
        // unspecified order.
        b = 15;

        //return 0;
    }

    // END EXAMPLE CODE

    ASSERT_TRUE("Hello world!\n10\nnot equal\n"s == cout_buf.str() ||
                "Hello world!\nnot equal\n10\n"s == cout_buf.str());
}
