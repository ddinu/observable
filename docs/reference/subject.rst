subject<>
=========

Subjects are the most basic observable objects. They allow you to subscribe
to notifications and to notify any subscribed observers when an event happens.

Observers can be any callable object (functors, function pointers or lambdas).

Example
-------

.. code-block:: C++

    #include <observable/subject.hpp>

    observable::subject<void(double)> subject;

    auto subscription = subject.subscribe([](double new_value) {
                                              // React to the notifcation.
                                          }); 

    subject.notify(5.1);

Class reference
---------------

.. doxygenclass:: observable::subject< void(Args...)>
    :members: