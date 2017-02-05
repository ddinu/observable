#pragma once
#include <functional>
#include <type_traits>

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

} }
