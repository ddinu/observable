value<>
==========

Observable values can be used to monitor changes to a value type.

They have public getters and public setters, so anyone with access to the
observable value instance can mutate it and trigger change notifications.

Example
-------

.. code-block:: C++

    #include <observable/value.hpp>
    using namespace observable;

    value<std::string> text;

    auto sub = text.subscribe([&](auto const & text) {
                                  // React to the text change.
                              });

    text = "foo";

Class reference
---------------

.. doxygenclass:: observable::value< ValueType, EqualityComparator >
    :members:

.. doxygenclass:: observable::value< ValueType, EqualityComparator, EnclosingType >
    :members: