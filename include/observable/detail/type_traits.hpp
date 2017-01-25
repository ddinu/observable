#pragma once
#include <functional>
#include <type_traits>

namespace observable { namespace detail {

//! Check if a callable type is compatible with an observer type.
template <typename CallableType, typename ObserverType>
using is_compatible_with_observer = std::is_convertible<
                                        CallableType,
                                        std::function<ObserverType>>;

//! Check if a callable type can be used to subscribe to a subject.
template <typename CallableType, typename SubjectType>
using is_compatible_with_subject = is_compatible_with_observer<
                                        CallableType,
                                        typename SubjectType::observer_type>;

} }
