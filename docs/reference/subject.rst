subject<>
=========

Subjects are the most basic observable objects. They allow you to subscribe
to notifications and to notify any subscribed observers when an event happens.

Observers can be any callable object (functors, function pointers or lambdas).

Class reference
---------------

.. doxygenclass:: observable::subject< void(Args ...)>
    :members: