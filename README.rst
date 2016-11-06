Observable: Generic observable objects for C++
==============================================

Did you ever needed to implement the observer pattern and didn't have an easy
way to do it? This is for those times.

Quick start
-----------

The library is header-only; just copy the ``include/observable`` directory into
your include path and you're set.

Observable properties:

.. code-block:: C++

    #include <observable/property.hpp>
    using namespace observable;

    class WidgetModel
    {
    public:
        property<std::string, WidgetModel> text;

        void set_text(std::string const & value) { text = value; }
    };

    WidgetModel widget_model;

    auto sub = widget_model.text.subscribe([&](std::string const & new_value) {
                                                /* Update the widget. */
                                           });
    widget_model.text.subscribe([&]() { /* React to updates */ }).release();

    widget_model.set_text("Hello!"); // Calls the lambdas above.

Simple subject:

.. code-block:: C++

    #include <observable/subject.hpp>

    observable::subject<void(double)> subject;

    auto subscription = subject.subscribe([](double value) {
                                              /* Use value */
                                          }); 
    subject.notify(5.1); // Calls the lambda from above.

Documentation
-------------

.. image:: https://readthedocs.org/projects/observable/badge/?version=latest
    :target: http://observable.readthedocs.io/en/latest/?badge=latest
    :alt: Documentation Status

The project is documented using `Sphinx <http://www.sphinx-doc.org/>`_ and
the documentation is hosted on `Read the Docs <https://readthedocs.org/>`.

You can `access the documentation <http://observable.readthedocs.io/en/latest/>`_ here: http://observable.readthedocs.io/en/latest/.

What's with the CMake files?
----------------------------

The library is using CMake to build and run the tests and benchmarks. You won't
need CMake if you don't want to run the tests.

Why not just use Boost.Signals2 or Qt?
--------------------------------------

Boost.Signals2 and Qt are pretty cool libraries and do their jobs well.

This library is not meant to replace signals and slots, it focuses more on 
providing easy to use observable properties for classes that are used as models 
in patterns like MVC.

Choose whichever library works best for your case; you can even choose them
both (for example, have your models use this library and your views use Qt). 

What's happening next
---------------------

 * Improved documentation.
 * Observable values (what properties are now, but with public setters). Properties
   will be based on them.
 * Observable collections (wrappers around the standard collections).
 * More complex code examples.

Contributing
------------

Bug reports, feature requests, documentation and code contributions are welcome and
highly appreciated.

Legal and Licensing
-------------------

The library is licensed under the `Apache License version 2.0 <LICENSE.txt>`_.

Supported compilers
-------------------

Any relatively recent compiler with C++14 support should work.

The code has been tested with the following compilers:

 * MSVC 14 (Visual Studio 2015)
 * GCC 5.4, 6.2
 * Clang 3.6, 3.7, 3.8, 3.9

Build status
------------

Visual Studio 2015 builds:

 * |win32 build|_ (32 bit)
 * |win64 build|_ (64 bit)

.. |win32 build| image:: https://ci.appveyor.com/api/projects/status/bee1g4nlh25olmct/branch/master?svg=true
.. _win32 build: https://ci.appveyor.com/project/ddinu/observable-xwigk/branch/master

.. |win64 build| image:: https://ci.appveyor.com/api/projects/status/abi5swnpvc2nof3r/branch/master?svg=true
.. _win64 build: https://ci.appveyor.com/project/ddinu/observable/branch/master

Clang and GCC (Linux) builds:

 * |linux build|_ (64 bit)

.. |linux build| image:: https://travis-ci.org/ddinu/observable.svg?branch=master
.. _linux build: https://travis-ci.org/ddinu/observable
