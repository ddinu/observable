Observable: Generic observable objects for C++
==============================================

Observable values and expressions for C++.

If you want to write code in a reactive style, this is for you.

.. code-block:: C++

    #include <iostream>
    #include "observable/observable.hpp"

    using namespace std;
    using namespace observable;

    int main()
    {
        auto a = value<int> { 5 };
        auto b = value<int> { 7 };
        auto avg = observe((a + b) / 2.0f);

        avg.subscribe([](auto const & val) { cout << val; });

        b = 15;
        // 10 will be printed on stdout.

        return 0;
    }

Check out the :doc:`getting-started` page for some more examples.

The library is usable on both Windows and Linux, with any recent compiler that
supports at least C++14.

The library does not have any external dependencies and you don't need to
compile anything (the library is header-only); so you can easily integrate it
into your project.

If you want to build the tests and benchmarks, they’re using CMake, so that
should be easy too.

.. toctree::
    :maxdepth: 2
    :glob:

    *
    Reference <https://danieldinu.com/observable/reference/group__observable.html>
