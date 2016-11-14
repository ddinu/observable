#pragma once
#include <algorithm>
#include <utility>
#include <type_traits>
#include "observable/compound_subject.hpp"

namespace observable {

//! Observable values provide a way to get notified when a value-type changes.
//!
//! When setting a new value, if the new value is different than the existing one,
//! any subscribed observers will be notified.
//!
//! All methods of this class can be safely called in parallel from multiple
//! threads.
//!
//! \tparam ValueType The value-type that will be stored inside the observable.
//! \tparam EqualityComparator A comparator to use when checking if new values
//!                            are different than the stored value.
template <typename ValueType, typename EqualityComparator=std::equal_to<ValueType>>
class value
{
public:
    //! Create a default-constructed observable value.
    //!
    //! Depending on the value type, the stored value will be either
    //! uninitialized or it will be default constructed.
    value() =default;

    //! Create an initialized observable value.
    //!
    //! \param initial_value The observable's initial value.
    explicit value(ValueType initial_value);

    //! Convert the observable value to its stored value type.
    operator ValueType() const;

    //! Retrieve the stored value.
    auto get() const -> ValueType const &;

    //! Subscribe to changes to the observable value.
    //!
    //! These subscriptions will be triggered whenever the stored value changes.
    //!
    //! \tparam Callable A callable taking no parameters or a callable taking one
    //!                  parameter that will be called with the new value:
    //!                  - ``void()`` -- will be called when the value changes but
    //!                    will not receive the new value.
    //!                  - ``void(T const &)`` or ``void(T)`` -- will be called
    //!                    with the new value. The expression
    //!                    ``T { value.get() }`` must be correct.
    template <typename Callable>
    auto subscribe(Callable && callable) -> unique_subscription;

    //! Set a new value, possibly notifying any subscribed observers.
    //!
    //! If the new value compares equal to the existing value, this method has no
    //! effect. The comparison is performed using the EqualityComparator.
    auto set(ValueType new_value) -> void;

    //! Set a new value. Will just call set().
    auto operator=(ValueType new_value) -> value &;

private:
    class data
    {
        ValueType stored_value;
        compound_subject<void(), void(ValueType const &)> value_changed;

        data() =default;

        data(ValueType initial_value) : stored_value { std::move(initial_value) }
        { }

        data(data const & other) : stored_value { other.stored_value }
        { }

        auto operator=(data const & other) -> data &
        {
            if(this != &other)
                stored_value = other.stored_value;

            return *this;
        }

        data(data && other) =default;

        auto operator=(data && other) -> data & =default;

        friend class value;
    } data_;
};

// Implementation

template <typename ValueType, typename EqualityComparator>
inline value<ValueType, EqualityComparator>::value(ValueType initial_value) :
    data_ { std::move(initial_value) }
{
}

template <typename ValueType, typename EqualityComparator>
inline value<ValueType, EqualityComparator>::operator ValueType() const
{
    return data_.stored_value;
}

template <typename ValueType, typename EqualityComparator>
inline auto value<ValueType, EqualityComparator>::get() const -> ValueType const &
{
    return data_.stored_value;
}

template <typename ValueType, typename EqualityComparator>
inline auto value<ValueType, EqualityComparator>::set(ValueType new_value) -> void
{
    static EqualityComparator const eq { };

    if(eq(new_value, data_.stored_value))
        return;

    data_.stored_value = std::move(new_value);
    data_.value_changed.notify();
    data_.value_changed.notify(get());
}

template <typename ValueType, typename EqualityComparator>
inline auto value<ValueType, EqualityComparator>::operator=(ValueType new_value) -> value &
{
    set(std::move(new_value));
    return *this;
}

template <typename ValueType, typename EqualityComparator>
template <typename Callable>
inline auto value<ValueType, EqualityComparator>::subscribe(Callable && callable) -> unique_subscription
{
    return data_.value_changed.subscribe(callable);
}

}
