#pragma once
#include <functional>
#include <type_traits>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable { namespace detail {

//! Check if a callable type is compatible with an observer type.
//!
//! A callable is compatible with an observer type if the callable can be stored
//! inside a `std::function<ObserverType>`.
//!
//! \tparam CallableType The type to check if it is compatible with the
//!                      ObserverType.
//! \tparam ObserverType The type of the observer to check against.
//!
//! The static member ``value`` will be true, if the CallableType is compatible
//! with the ObserverType.
//!
//! \ingroup observable_detail
template <typename CallableType, typename ObserverType>
using is_compatible_with_observer = std::is_convertible<
                                        CallableType,
                                        std::function<ObserverType>>;

//! Check if a callable type can be used to subscribe to a subject.
//!
//! A callable can be used to subscribe to a subject if the callable is compatible
//! with the subject's ``observer_type`` type.
//!
//! \tparam CallableType The type to check if it can be used to subscribe to the
//!                      provided subject.
//! \tparam SubjectType The subject to check against.
//!
//! The static member ``value`` will be true if, the CallableType can be used
//! with the SubjectType.
//!
//! \ingroup observable_detail
template <typename CallableType, typename SubjectType>
using is_compatible_with_subject = is_compatible_with_observer<
                                        CallableType,
                                        typename SubjectType::observer_type>;

//! Check if two types are equality comparable between themselves.
template<typename A, typename B, typename = void>
struct are_equality_comparable : std::false_type
{ };

//! \cond
template<typename A, typename B>
struct are_equality_comparable<A, B, std::enable_if_t<
                                        std::is_same<
                                            std::decay_t<
                                                decltype(std::equal_to<> { }(std::declval<A>(),
                                                                             std::declval<B>()))
                                             >, bool>::value>
        > : std::true_type
{
};
//! \endcond

} }

OBSERVABLE_END_CONFIGURE_WARNINGS
