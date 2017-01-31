#include <iostream>
#include <string>
#include "observable/observable.hpp"
#include "gtest/gtest.h"
#include "utility.h"

using namespace std::string_literals;

TEST(example_tests, getting_started_with_subjects)
{
    provide_cin cin_buf { "Jane"s };
    capture_cout cout_buf;

    // BEGIN EXAMPLE CODE

    using namespace std;
    using namespace observable;

    // Will print "Hello {name}" each time you enter a name.
    // Type exit to stop.

    //int main()
    {
        auto name_entered = subject<void(string)> { };

        // Subscribe to name_entered notifications.
        name_entered.subscribe([](auto const & name) {
                                    cout << "Hello "s
                                        << name
                                        << "!"s
                                        << endl;
                                });

        while(cin)
        {
            cout << "What's your name?"s << endl;

            // Read the name.
            auto input_name = string { };
            cin >> input_name;

            if(input_name.empty() || input_name == "exit"s)
                break;

            // Notify all name_entered observers.
            name_entered.notify(input_name);
        }

        //return 0;
    }

    // END EXAMPLE CODE

    ASSERT_EQ("What's your name?\nHello Jane!\nWhat's your name?\n"s,
              cout_buf.str());
}

TEST(example_tests, getting_started_with_values)
{
    provide_cin cin_buf { "Jane"s };
    capture_cout cout_buf;

    // BEGIN EXAMPLE CODE

    using namespace std;
    using namespace observable;

    // Will print "Hello {name}" each time you enter a name.
    // Type exit to stop.

    //int main()
    {
        auto name = value<string> { };

        // Recompute the greeting each time name changes and
        // fire change notifications.
        auto greeting = observe("Hello "s + name + "!"s);

        // Subscribe to greeting changes.
        greeting.subscribe([](auto const & name) {
                               cout << name << endl;
                           });

        while(cin)
        {
            cout << "What's your name?"s << endl;

            // Read the name.
            auto input_name = string { };
            cin >> input_name;

            if(input_name.empty() || input_name == "exit"s)
                break;

            // Update the name value.
            name = input_name;
        }

        //return 0;
    }

    // END EXAMPLE CODE

    ASSERT_EQ("What's your name?\nHello Jane!\nWhat's your name?\n"s,
              cout_buf.str());
}

TEST(example_tests, getting_started_with_properties)
{
    provide_cin cin_buf { "Jane"s };
    capture_cout cout_buf;

    // BEGIN EXAMPLE CODE

    using namespace std;
    using namespace observable;

    // Will print "Hello {name}" each time you enter a name.
    // Type exit to stop.

    // Greet people using names read from stdin.
    class NameModel
    {
        OBSERVABLE_PROPERTIES(NameModel)

    public:
        // Current name.
        observable_property<string> name;

        // Current greeting.
        observable_property<string> greeting = observe(
                                                   "Hello "s + name + "!"s
                                               );

    public:
        // Read names from stdin until the user quits.
        void read_names()
        {
            while(cin)
            {
                cout << "What's your name?"s << endl;

                auto input_name = string { };
                cin >> input_name;

                if(input_name.empty() || input_name == "exit"s)
                    break;

                name = input_name;
            }
        }
    };

    //int main()
    {
        NameModel model;

        // Print the greetings.
        model.greeting.subscribe([](auto const & hello) {
                                    cout << hello << endl;
                                 });

        // Properties cannot be set from outside the class. The
        // line below will not compile:
        // model.name = input_name;

        model.read_names();
        //return 0;
    }

    // END EXAMPLE CODE

    ASSERT_EQ("What's your name?\nHello Jane!\nWhat's your name?\n"s,
              cout_buf.str());
}
