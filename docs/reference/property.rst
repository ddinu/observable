property<>
==========

Properties can be used to monitor changes to a class member. They have public
getters but private setters that can only be access from the enclosing class.

Example
-------

.. code-block:: C++

    #include <observable/property.hpp>
    using namespace observable;

    class MyModel
    {
    public:
        property<std::string, MyModel> text;

        void set_text(std::string const & value) { text = value; }
    };

    MyModel model;
    // model.text = "foo"; will not compile

    auto sub = model.text.subscribe([&](auto const & text) {
                                        // React to the text change.
                                    });

    model.set_text("foo");

Class reference
---------------

.. doxygenclass:: observable::property
    :members:
    :private-members: