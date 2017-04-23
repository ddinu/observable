Getting started
===============

The library has a few classes and a set of macros and function overloads:

`subject\<void(Args ...)\>`_
    Provides a way to notify subscribed observers when an event occurs.

`value\<T\>`_
    Holds value types and provides a way to notify subscribed observers when
    the value changes.

`OBSERVABLE_PROPERTIES`_ and `observable_property`_
    Values nested inside an enclosing class with setters accessible only from
    inside that class.

`observe()`_
    A group of overloaded functions that can create observable values that are
    updated when their argument expression changes.

.. _subject\<void(Args ...)\>: reference/classobservable_1_1subject_3_01void_07_args_01_8_8_8_08_4.html
.. _value\<T\>: reference/classobservable_1_1value_3_01_value_type_00_01_equality_comparator_01_4.html
.. _OBSERVABLE_PROPERTIES: reference/group__observable.html#ga29f96693ca8b710b884b72860149fb7b
.. _observable_property: reference/group__observable.html#gacca2b9245c501c1f5f71fabd516a66d3
.. _observe(): reference/group__observable.html#ga25c1181fc75df6d45c0e8da530ce8639

Getting started with subjects
-----------------------------

Subjects are the simplest observable objects. They provide a way to get notified
when some event occurs.

.. code-block:: C++

    #include <iostream>
    #include <string>
    #include <observable/observable.hpp>

    using namespace std;
    using namespace observable;

    // Will print "Hello {name}" each time you enter a name.
    // Type exit to stop.

    int main()
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

        return 0;
    }

Getting started with values
---------------------------

Values are wrappers around value-types. You can subscribe to changes to the
contained value-type.

The ``observe()`` functions can be used to generate values associated with
an expression. For example: ``auto result = observe((a + b) / 2.0)``.
The ``result`` value will recompute the average, each time either the ``a``
or ``b`` value changes.

.. code-block:: C++

    #include <iostream>
    #include <string>
    #include <observable/observable.hpp>

    using namespace std;
    using namespace observable;

    // Will print "Hello {name}" each time you enter a name.
    // Type exit to stop.

    int main()
    {
        auto name = value<string> { };

        // Recompute the greeting each time name changes and
        // fire change notifications.
        auto greeting = observe("Hello "s + name + "!"s);

        // Subscribe to greeting changes.
        greeting.subscribe([](auto const & hello) {
                               cout << hello << endl;
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

        return 0;
    }

Getting started with observable properties
------------------------------------------

Properties are just values that are nested inside a class. This makes all
setters inaccessible from outside the enclosing class.

.. code-block:: C++

    #include <iostream>
    #include <string>
    #include <observable/observable.hpp>

    using namespace std;
    using namespace observable;

    // Will print "Hello {name}" each time you enter a name.
    // Type exit to stop.

    // Greet people using names read from stdin.
    class Greeter
    {
        OBSERVABLE_PROPERTIES(Greeter)

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

    int main()
    {
        Greeter greeter;

        // Print the greetings.
        greeter.greeting.subscribe([](auto const & hello) {
                                       cout << hello << endl;
                                   });

        // Properties cannot be set from outside the class. The
        // line below will not compile:
        // greeter.name = input_name;

        greeter.read_names();
        return 0;
    }
