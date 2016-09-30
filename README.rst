Observable: Generic observable objects for C++
==============================================

Did you ever needed to implement the observer pattern and didn't have an easy
way to do it? This is for those times.

Quick start
-----------

The library is header-only; just copy the ``include/observable`` directory into
your include path and you're set.

Simple subject:

.. code-block:: C++

    #include <observable/subject.hpp>

    observable::subject<void(double)> subject;

    auto subscription = subject.subscribe([](double frob) { /* do stuff */ }); 
    subject.notify(5.1); // Calls 'do stuff'

Properties:

.. code-block:: C++

    #include <observable/property.hpp>

    class Frobulator
    {
    public:
        observable::property<int, Frobulator> my_value;

        void frobulate(int value) { my_value = value; }
    };

    Frobulator frobulator;

    auto sub = frobulator.my_value.subscribe([](int newValue) { /* do stuff */ });
    frobulator.my_value.subscribe([]() { /* do some more stuff */ }).release();

    frobulator.frobulate(5); // Calls 'do stuff' and 'do some more stuff'.

Documentation
-------------

The project is documented using `Sphinx <http://www.sphinx-doc.org/>`_.

You can `access the documentation <docs/index.rst>`_ under the docs directory.

What's with the CMake files?
----------------------------

The library is using CMake to build and run the tests and benchmarks. You won't
need CMake if you don't want to run the tests.

Warning!
--------

This is a work-in-progress. Don't use it in production.

Contributing
------------

Any help is welcome.

Legal and Licensing
-------------------

The library is licensed under the `Apache License version 2.0 <LICENSE.txt>`_.

Supported compilers
-------------------

Any relatively recent compiler with C++14 support should work.

The code has been tested with the following compilers:

 * MSVC 14 (Visual Studio 2015)
 * GCC 5.4
 * Clang 3.8

Build status
------------

Visual Studio 2015 builds:

 * |win32 build|_ (32 bit)
 * |win64 build|_ (64 bit)

.. |win32 build| image:: https://ci.appveyor.com/api/projects/status/bee1g4nlh25olmct/branch/master?svg=true
.. _win32 build: https://ci.appveyor.com/project/ddinu/observable-xwigk/branch/master

.. |win64 build| image:: https://ci.appveyor.com/api/projects/status/abi5swnpvc2nof3r/branch/master?svg=true
.. _win64 build: https://ci.appveyor.com/project/ddinu/observable/branch/master
