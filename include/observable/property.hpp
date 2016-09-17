#pragma once
#include <algorithm>
#include <utility>
#include <type_traits>
#include "observable/compound_subject.hpp"

namespace observable {

//! Property that can notify observers when its value changes.
//!
//! \tparam ValueType Type that will be stored in the property.
//! \tparam EnclosingType The type that will have access to the property setter.
//!                       Only this type will be able to change the property's
//!                       value.
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

    //! Create a property that has a value initialized to the one specified.
    //!
    //! \param initial_value The property will be initialized to this value.
    explicit property(ValueType initial_value);

    //! Convert the property to its stored value.
    operator ValueType() const;

    //! Retrieve the stored value.
    auto value() const -> ValueType const &;

    //! Subscribe to changes to the property's value.
    //!
    //! These subscriptions will be triggered whenever the stored value changes.
    //! Values are compared using the EqualityComparator.
    template <typename Callable>
    auto subscribe(Callable && callable) -> unique_subscription;

private:
    //! Set a new value for the property.
    //!
    //! If the new value compares equal to the old value, this method has no
    //! effect. The comparison is performed using the EqualityComparator.
    auto set(ValueType new_value) -> void;

    //! Set a new value for the property. Just calls set().
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

template <typename ValueType, typename EnclosingType, typename EqualityComparator>
inline property<ValueType, EnclosingType, EqualityComparator>::property(ValueType initial_value) :
    data_ { std::move(initial_value) }
{
}

template <typename ValueType, typename EnclosingType, typename EqualityComparator>
inline property<ValueType, EnclosingType, EqualityComparator>::operator ValueType() const
{
    return data_.value;
}

template <typename ValueType, typename EnclosingType, typename EqualityComparator>
inline auto property<ValueType, EnclosingType, EqualityComparator>::value() const -> ValueType const &
{
    return data_.value;
}

template <typename ValueType, typename EnclosingType, typename EqualityComparator>
inline auto property<ValueType, EnclosingType, EqualityComparator>::set(ValueType new_value) -> void
{
    static EqualityComparator eq;

    if(eq(new_value, data_.value))
        return;

    data_.value = std::move(new_value);
    data_.value_change.notify();
    data_.value_change.notify(value());
}

template <typename ValueType, typename EnclosingType, typename EqualityComparator>
inline auto property<ValueType, EnclosingType, EqualityComparator>::operator=(ValueType value) -> property &
{
    set(std::move(value));
    return *this;
}

template <typename ValueType, typename EnclosingType, typename EqualityComparator>
template <typename Callable>
inline auto property<ValueType, EnclosingType, EqualityComparator>::subscribe(Callable && callable) -> unique_subscription
{
    return data_.value_change.subscribe(callable);
}

}
