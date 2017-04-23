Observable: Generic observable objects for C++
==============================================

If you want to write code in a reactive style or just implement the observer
pattern, this is for you.

Quick start
-----------

The library is header-only and has no dependencies; copy the
``include/observable`` directory into your include path and you're set.

Example:

.. code:: C++

    #include <iostream>
    #include <observable/observable.hpp>

    using namespace std;
    using namespace observable;

    int main()
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

        return 0;
    }

Documentation
-------------

You can `access the documentation <https://danieldinu.com/observable/>`_ here:
https://danieldinu.com/observable/.

What's with the CMake files?
----------------------------

The library is using CMake to build the tests, benchmarks and documentation. You
won't need CMake if you don't want to work on the library as a developer.

Why not just use Boost.Signals2 or Qt?
--------------------------------------

Boost.Signals2 and Qt are pretty cool libraries and do their jobs well.

This library is not meant to replace signals and slots, it focuses more on 
providing easy to use observable objects and expressions that can help with
patterns like MVC and reactive programming.

Choose whichever library works best for your case; you can even choose them
both (for example, have your models use this library and your views use Qt). 

Contributing
------------

Bug reports, feature requests, documentation and code contributions are welcome 
and highly appreciated.

Legal and Licensing
-------------------

The library is licensed under the `Apache License version 2.0 <LICENSE.txt>`_.

Please note that all contributions are considered to be provided under the
terms of this license.

Supported compilers
-------------------

Any relatively recent compiler with C++14 support should work.

The code has been tested with the following compilers:

 * MSVC 15 (Visual Studio 2017)
 * MSVC 14 (Visual Studio 2015)
 * GCC 5.4, 6.2
 * Clang 3.6, 3.7, 3.8, 3.9

Build status
------------

Visual Studio 2017 builds:

 * |win32 15 build|_ (32 bit)
 * |win64 15 build|_ (64 bit)

.. |win32 15 build| image:: https://ci.appveyor.com/api/projects/status/sgomsxwri8wknode?svg=true
.. _win32 15 build: https://ci.appveyor.com/project/ddinu/observable-crrsf

.. |win64 15 build| image:: https://ci.appveyor.com/api/projects/status/tpr4qem5gxo7dntb?svg=true
.. _win64 15 build: https://ci.appveyor.com/project/ddinu/observable-uyjd7

Visual Studio 2015 builds:

 * |win32 14 build|_ (32 bit)
 * |win64 14 build|_ (64 bit)

.. |win32 14 build| image:: https://ci.appveyor.com/api/projects/status/bee1g4nlh25olmct/branch/master?svg=true
.. _win32 14 build: https://ci.appveyor.com/project/ddinu/observable-xwigk

.. |win64 14 build| image:: https://ci.appveyor.com/api/projects/status/abi5swnpvc2nof3r/branch/master?svg=true
.. _win64 14 build: https://ci.appveyor.com/project/ddinu/observable

Linux (GCC, Clang) and OS X (Clang) builds:

 * |travis build|_ (64 bit)

.. |travis build| image:: https://travis-ci.org/ddinu/observable.svg?branch=master
.. _travis build: https://travis-ci.org/ddinu/observable
