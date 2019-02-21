Observable: Generic observable objects for C++
==============================================

Write declarative, reactive expressions or just implement the observer pattern.

Observable is a self-contained, header-only library that has no depencencies.
Drop it somewhere in your include path and you're good to go.

Quick start
-----------

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

        // "Hello world!" will be printed to stdout.
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

        // "10" and "not equal" will be printed to stdout in an
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

The library uses CMake to build the tests, benchmarks and documentation. You
do not need CMake if you don't plan on running the tests or benchmarks.

Contributing
------------

Bug reports, feature requests, documentation and code contributions are welcome 
and highly appreciated. Please open an issue or feature request before you
start working on any pull request.

Legal and Licensing
-------------------

The library is licensed under the `Apache License version 2.0 <LICENSE.txt>`_.

All contributions must be provided under the terms of this license.

Supported compilers
-------------------

Any relatively recent compiler with C++14 support should work.

The code has been tested with the following compilers:

* MSVC 15 (Visual Studio 2017)
* MSVC 14 (Visual Studio 2015)
* GCC 5, 6, 7
* Clang 3.6, 3.8
* AppleClang 9.1

Build status
------------

Visual Studio 2017 builds:

 * |win32 15 build C++14|_ (32 bit, C++14)
 * |win64 15 build C++14|_ (64 bit, C++14)
 * |win32 15 build C++17|_ (32 bit, C++17)
 * |win64 15 build C++17|_ (64 bit, C++17)

.. |win32 15 build C++14| image:: https://ci.appveyor.com/api/projects/status/sgomsxwri8wknode?svg=true
.. _win32 15 build C++14: https://ci.appveyor.com/project/ddinu/observable-crrsf

.. |win64 15 build C++14| image:: https://ci.appveyor.com/api/projects/status/tpr4qem5gxo7dntb?svg=true
.. _win64 15 build C++14: https://ci.appveyor.com/project/ddinu/observable-uyjd7

.. |win32 15 build C++17| image:: https://ci.appveyor.com/api/projects/status/296i1mvgm7fht0f6?svg=true
.. _win32 15 build C++17: https://ci.appveyor.com/project/ddinu/observable-2lmia

.. |win64 15 build C++17| image:: https://ci.appveyor.com/api/projects/status/i948buecj8j51by0?svg=true
.. _win64 15 build C++17: https://ci.appveyor.com/project/ddinu/observable-ha4xx

Visual Studio 2015 builds:

 * |win32 14 build|_ (32 bit, C++14)
 * |win64 14 build|_ (64 bit, C++14)

.. |win32 14 build| image:: https://ci.appveyor.com/api/projects/status/bee1g4nlh25olmct/branch/master?svg=true
.. _win32 14 build: https://ci.appveyor.com/project/ddinu/observable-xwigk

.. |win64 14 build| image:: https://ci.appveyor.com/api/projects/status/abi5swnpvc2nof3r/branch/master?svg=true
.. _win64 14 build: https://ci.appveyor.com/project/ddinu/observable

Linux (GCC, Clang) and OS X (Clang) builds:

 * |travis build|_ (64 bit)

.. |travis build| image:: https://travis-ci.org/ddinu/observable.svg?branch=master
.. _travis build: https://travis-ci.org/ddinu/observable
