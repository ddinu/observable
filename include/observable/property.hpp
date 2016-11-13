#pragma once
#include <algorithm>
#include <utility>
#include <type_traits>
#include "observable/compound_subject.hpp"

namespace observable {

//! Properties provide a way to get notified when a member's value changes.
//!
//! The property class is meant to be used as a public or protected member inside
//! another class.
//!
//! The property's setters are declared private but the enclosing class is
//! declared friend, essentially restricting access for mutating the property
//! to all code except the one contained in the enclosing class.
//!
//! When setting a new value, if the new value is different than the existing one,
//! any subscribed observers will be notified.
//!
//! All methods of this class can be safely called in parallel from multiple
//! threads.
//!
//! \tparam ValueType The type that will be stored in the property.
//! \tparam EnclosingType The type that will have access to the property setters.
//!                       Only this type will be able to change the property's
//!                       value. This type will be declared as friend.
//! \tparam EqualityComparator A comparator to use when checking if new values
//!                            are different than the stored value.
template <typename ValueType,
          typename EnclosingType=void,
          typename EqualityComparator=std::equal_to<ValueType>>
class property
{
public:
    //! Create a default-constructed property.
    //!
    //! Depending on the value type, the property's value will be either
    //! uninitialized or it will be default constructed.
    property() =default;

    //! Create a property that has a value initialized with the one provided.
    //!
    //! \param initial_value The property will be initialized with this value.
    explicit property(ValueType initial_value);

    //! Convert the property to its stored value.
    operator ValueType() const;

    //! Retrieve the stored value.
    auto value() const -> ValueType const &;

    //! Subscribe to changes to the property's value.
    //!
    //! These subscriptions will be triggered whenever the stored value changes.
    //!
    //! \tparam Callable A callable taking no parameters or a callable taking one
    //!                  parameter that will be called with the new value:
    //!                  - ``void()`` -- will be called when the value changes but
    //!                    will not receive the new value.
    //!                  - ``void(T const &)`` or ``void(T)`` -- will be called
    //!                    with the new value. The expression
    //!                    ``T { property.value() }`` must be correct.
    template <typename Callable>
    auto subscribe(Callable && callable) -> unique_subscription;

private:
    //! Set a new value for the property.
    //!
    //! If the new value compares equal to the old value, this method has no
    //! effect. The comparison is performed using the EqualityComparator.
    //!
    //! \note This method will be accessible from inside the EnclosingType class.
    auto set(ValueType new_value) -> void;

    //! Set a new value for the property. Just calls set().
    //!
    //! \note This method will be accessible from inside the EnclosingType class.
    auto operator=(ValueType value) -> property &;

    friend EnclosingType;

private:
    class data
    {
        ValueType value;
        compound_subject<void(), void(ValueType const &)> value_change;

        data() =default;

        data(ValueType initial_value) : value { std::move(initial_value) }
        { }

        data(data const & other) : value { other.value }
        { }

        auto operator=(data const & other) -> data &
        {
            if(this != &other)
                value = other.value;

            return *this;
        }

        data(data && other) =default;

        auto operator=(data && other) -> data & =default;

        friend class property;
    } data_;
};

// Implementation

template <typename ValueType,
          typename EnclosingType,
          typename EqualityComparator>
inline property<ValueType,
                EnclosingType,
                EqualityComparator>::property(ValueType initial_value) :
    data_ { std::move(initial_value) }
{
}

template <typename ValueType,
          typename EnclosingType,
          typename EqualityComparator>
inline property<ValueType,
                EnclosingType,
                EqualityComparator>::operator ValueType() const
{
    return data_.value;
}

template <typename ValueType,
          typename EnclosingType,
          typename EqualityComparator>
inline auto property<ValueType,
                     EnclosingType,
                     EqualityComparator>::value() const -> ValueType const &
{
    return data_.value;
}

template <typename ValueType,
          typename EnclosingType,
          typename EqualityComparator>
inline auto property<ValueType,
                     EnclosingType,
                     EqualityComparator>::set(ValueType new_value) -> void
{
    static EqualityComparator eq;

    if(eq(new_value, data_.value))
        return;

    data_.value = std::move(new_value);
    data_.value_change.notify();
    data_.value_change.notify(value());
}

template <typename ValueType,
          typename EnclosingType,
          typename EqualityComparator>
inline auto property<ValueType,
                     EnclosingType,
                     EqualityComparator>::operator=(ValueType value) -> property &
{
    set(std::move(value));
    return *this;
}

template <typename ValueType,
          typename EnclosingType,
          typename EqualityComparator>
template <typename Callable>
inline auto property<ValueType,
                     EnclosingType,
                     EqualityComparator>::subscribe(Callable && callable) -> unique_subscription
{
    return data_.value_change.subscribe(callable);
}

}
