Observable: Generic observable objects for C++
==================================

Did you ever needed to implement the observer pattern and didn't have an easy
way to do it? This is for those times.

Quick start
-----------

The library is header-only, just copy the `include/observable` directory into
your include path and you're set.

Example:

.. code-block:: C++

    #include <observable/subject.hpp>

    subject<> observable;

    observable.subscribe([]() { /* do stuff 1 */ });
    observable.subscribe([](double frob) { /* do stuff 2 */ }); 
    observable.subscribe("stopped", [](double last_frob) { /* do stuff 3 */ });

    observable.notify(); // calls 'do stuff 1'
    observable.notify(5.0); // calls 'do stuff 2'
    observable.notify_tagged("stopped", 5.0); // calls 'do stuff 3'

What the library can do
-----------------------

You can subscribe to notifications using generic functors, functions or
lambdas and can fire notifications that call your subscribed 'observers'.

Only functions that have the correct number and types of parameters get called.

If you use `notify_tagged()`, the tag used when subscribing also has to match
the one used for the notification.

What's with the CMake files?
----------------------------

The library is using CMake to build and run the tests. You won't need CMake
if you don't want to run the tests.

Warning!
--------

This is a work-in-progress. Don't use it in production.

Contributing
------------

Any help is welcome.

Legal and Licensing
-------------------

Observable is licensed under the `Apache License version 2.0 <LICENSE.txt>`_.