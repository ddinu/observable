How to use expressions
======================

Observable expressions are a simple way of composing `observable values`_ and
constants.

.. _`observable values`: ../reference/classobservable_1_1value_3_01_value_type_00_01_equality_comparator_01_4.html

Simple expressions
------------------

You can use operators, like ``+`` and ``*``, to combine observable values into
arbitrary expressions. These expressions are converted to values by passing them
to the `observe()`_ functions.

.. _`observe()`: ../reference/group__observable.html#ga25c1181fc75df6d45c0e8da530ce8639

.. note:: If the observable value's contained type does not support an operator,
          neither will the observable value.

Here's a small example:

.. code-block:: C++

    #include <cmath>
    #include <observable/observable.hpp>

    int main()
    {
        auto radius = observable::value<double> { 5 };
        auto circumference = observe(2 * M_PI * radius);

        assert(circumference.get() == 2 * M_PI * 5);

        radius = 7;

        assert(circumference.get() == 2 * M_PI * 7);
    }

The ``radius`` variable is an observable value. Because of that, it can be used
in an expression, which, when passed to observe, produces another value:
``circumference``.

The ``circumference`` value will be updated each time ``radius`` gets changed.

Using updaters
--------------

In the previous example, the update is automatic and immediate, but you can
control when the expression is recomputed by passing an `updater`_ to
`observe()`_.

.. _`updater`: ../reference/classobservable_1_1updater.html

Here's how using an `updater`_ works:

.. code-block:: C++

    #include <cmath>
    #include <observable/observable.hpp>

    int main()
    {
        auto radius = observable::value<double> { 5 };

        auto updater = observable::updater { };
        auto circumference = observe(updater, 2 * M_PI * radius);

        assert(circumference.get() == 2 * M_PI * 5);

        radius = 7;
        assert(circumference.get() == 2 * M_PI * 5);

        updater.update_all();
        assert(circumference.get() == 2 * M_PI * 7);
    }

When you call ``update_all()``, all values returned by ``observe()`` calls,
which got passed the updater instance, will be updated with the new result
of the expression.

By using updaters, you can have complete control over when expressions
are evaluated.

You can use multiple updaters at the same time.

Another benefit of using updaters is that your expression will not be evaluated
multiple times, because you can delay the call to ``update_all()`` until
you know that all values that will be changed, have changed.

Expression filters
------------------

An expression filter is a callable object that can take one or more expression
nodes as parameters and returns an expression node [#]_. Basically, it's a
function that can be used inside expressions.

.. [#] The low-level components of an expression are called *expression nodes*.

Predefined filters
++++++++++++++++++

There are a number of `predefined filters <../reference/group__observable__expressions.html>`_,
that you can check out in the `reference <../reference/group__observable__expressions.html>`_.

Like with operators, you can take advantage of `ADL`_ and just use the filter's
unqualified name.

.. _`ADL`: http://en.cppreference.com/w/cpp/language/adl

.. code-block:: C++

    #include <cmath>
    #include <observable/observable.hpp>

    int main()
    {
        auto radius = observable::value<double> { 5 };
        auto area = observe(M_PI * pow(radius, 2));
        auto is_large = observe(select(area > 100, true, false));

        assert(area.get() == M_PI * std::pow(5, 2));
        assert(is_large.get() == false);

        radius = 70;

        assert(area.get() == M_PI * std::pow(70, 2));
        assert(is_large.get() == true);
    }

User defined filters
++++++++++++++++++++

You can write your own expression filters.

It's pretty easy as you won't need to handle the expression nodes directly;
just write a normal function taking the right values and use the
`OBSERVABLE_ADAPT_FILTER <../reference/group__observable__expressions.html#ga06de81bd93a814eefde0b3ba3118d3fe>`_
macro.

The predefined filters are created with the same macro.

Let's take a look at an example:

.. code-block:: C++

    #include <cmath>
    #include <observable/observable.hpp>

    double square_(double val) { return std::pow(val, 2); }
    OBSERVABLE_ADAPT_FILTER(square, square_)

    int main()
    {
        auto radius = observable::value<double> { 5 };
        auto area = observe(M_PI * square(radius));

        assert(area.get() == M_PI * std::pow(5, 2));

        radius = 70;

        assert(area.get() == M_PI * std::pow(70, 2));
    }

The function that you provide to the macro will be called each time
the expression is evaluated, so keep it fast.

The filter will be declared in the same namespace where the macro is used.

Conclusion
----------

Instead of using subscribe and callbacks, expressions are an easy way of
constructing and updating values.

Check out the `expression reference <../reference/group__observable__expressions.html>`_
for more details.