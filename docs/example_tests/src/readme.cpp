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

    // Will print "Hello {name}!" for each name that you enter.
    // Use "exit" to stop.

    //int main()
    {
        // Event will be fired before exiting.
        auto before_exit = subject<void()> { };
        before_exit.subscribe([]() { cout << "Bye!"s << endl; });

        // Current name.
        auto name = value<string> { };

        // Current greeting.
        auto greeting = observe("Hello "s + name + "!"s);
        greeting.subscribe([](auto const & hello) {
            cout << hello << endl;
        });

        // Read the names.
        while(cin)
        {
            cout << "What's your name?"s << endl;

            // Read the name.
            auto input_name = string { };
            cin >> input_name;

            if(input_name.empty() || input_name == "exit"s)
                break;

            // Update the current name.
            name = input_name;
        }

        // Notify observers that we're exiting.
        before_exit.notify();

        //return 0;
    }

    // END EXAMPLE CODE

    ASSERT_EQ("What's your name?\nHello Jane!\nWhat's your name?\nBye!\n"s,
              cout_buf.str());
}
