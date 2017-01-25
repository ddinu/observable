Getting started
===============

The library has two main classes: :doc:`value\<\> </reference/value>` and
:doc:`subject\<\> </reference/subject>`.

A value is a way to notify subscribed observers that a variable has changed,
while a subject is just a way to notify observers that an event has occured.

Simple value example
-----------------------

.. code-block:: C++

    #include <observable/value.hpp>
    using namespace observable;

    class WidgetModel
    {
    public:
        property<std::string, WidgetModel> text;

        void set_text(std::string const & value) { text = value; }
    };

    WidgetModel widget_model;
    WidgetView widget_view;

    auto sub = widget_model.text.subscribe([&](auto const & text) {
                                               widget_view.set_text(text);
                                           });

    widget_model.text.subscribe([]() { /* React to updates */ }).release();

    widget_model.set_text("Hello!"); // Calls the lambdas above.

    // widget_model.text = "bar"; // Will not compile.

.. NOTE::

    ``observable::property<>`` is just an alias for ``observable::value<>``.

    By providing the enclosing ``WidgetModel`` class as a template parameter,
    you'll be able to use the assignment operator from inside your class, but
    other code will not be able to access them.

Simple subject example
----------------------

.. code-block:: C++

    #include <observable/subject.hpp>
    using namespace observable;

    subject<void(double)> some_event;

    auto subscription = some_event.subscribe([](double value) {
                                                /* Use value */
                                             }); 

    some_event.notify(5.1); // Calls the lambda from above.

What's going on
---------------

When you subscribe to either subjects or values, you get back a
:doc:`unique_subscription </reference/unique_subscription>` object, which you
can use to unsubscribe.

:doc:`unique_subscription </reference/unique_subscription>` objects, act like a
``unique_ptr``, but instead of deleting something, they unsubscribe your
observer when they go out of scope.

You can also create :doc:`shared_subscription </reference/shared_subscription>`
objects (from unique ones). These behave like ``shared_ptr`` and unsubscribe
your observer when the last instance goes out of scope.

If you want subscriptions that live forever, just call ``release()`` on the
subscription.

You generally don't have to worry about lifetimes, except to keep the subscribed
observer valid for as long as there is a chance that it will be notified.
