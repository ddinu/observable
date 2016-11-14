#pragma once
#include <algorithm>
#include <utility>
#include <type_traits>
#include "observable/value.hpp"

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
class property : public value<ValueType, EqualityComparator>
{
public:
    using value<ValueType, EqualityComparator>::value;

private:
    //! Set a new value for the property.
    //!
    //! If the new value compares equal to the old value, this method has no
    //! effect. The comparison is performed using the EqualityComparator.
    //!
    //! \note This method will be accessible from inside the EnclosingType class.
    auto set(ValueType new_value)
    {
        value<ValueType, EqualityComparator>::set(std::move(new_value));
    }

    //! Set a new value for the property. Just calls set().
    //!
    //! \note This method will be accessible from inside the EnclosingType class.
    auto operator=(ValueType new_value)
    {
        return value<ValueType, EqualityComparator>::operator=(std::move(new_value));
    }

    friend EnclosingType;
};

}
