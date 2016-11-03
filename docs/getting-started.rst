Getting started
===============

The library has two main classes: ``subject<>`` and ``property<>``.

A subject is just a way to notify subscribers that an event has occured and
a property is a way to notify subscribers that a class member has changed.

Simple subject example
----------------------

.. code-block:: C++

    #include <observable/subject.hpp>

    observable::subject<void(double)> subject;

    auto subscription = subject.subscribe([](double value) {
                                              /* Use value */
                                          }); 
    subject.notify(5.1); // Calls the lambda from above.

Simple property example
-----------------------

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

    auto sub = widget_model.text.subscribe([](std::string const & new_value) {
                                                /* Update the widget. */
                                           });
    widget_model.text.subscribe([]() { /* React to updates */ }).release();

    widget_model.set_text("Hello!"); // Calls the lambdas above.

Note the second template parameter for the ``text`` property above. Property setters are private and by providing the enclosing (``WidgetModel``) class as a template parameter, you'll be able to use the assignment operator.

Quick description
-----------------

When you subscribe to either subjects or properties, you get back a subscription
object which you can use to unsubscribe.

Subscription objects act like a shared pointer. They will unsubscribe your
observer when the last subscription instance one is destroyed.

If you want subscriptions that live forever, just call
``subscription.release()``.

You generally don't have to worry about lifetimes, except to keep the
subscribed callable valid for as long as the subscription is active.
