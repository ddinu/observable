subject<>
=========

Subjects are the most basic observable objects. They allow you to subscribe
to notifications and to notify any subscribed observers when an event happens.

Observers can be any callable object (functors, function pointers or lambdas).

Example
-------

.. code-block:: C++

    #include <observable/subject.hpp>
    using namespace observable;

    subject<void(double)> some_event;

    auto sub = some_event.subscribe([](double new_value) {
                                        // React to the notifcation.
                                    }); 

    some_event.notify(5.1);

Class reference
---------------

.. doxygenclass:: observable::subject< void(Args...)>
    :members:

.. doxygenclass:: observable::subject< ObserverType, EnclosingType >
    :members: