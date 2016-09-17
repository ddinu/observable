#include <tuple>
#include <type_traits>
#include <functional>
#include "observable/subject.hpp"
#include "observable/detail/compound_subject_utility.hpp"

namespace observable {

//! A compound subject can notify multiple types of observers.
//!
//! \tparam FunctionTypes All function types that this subject will notify.
//! \sa subject
//! \note All methods of this class can be safely called in parallel.
template <typename ... FunctionTypes>
class compound_subject final
{
public:
    //! Subscribe to notifications.
    template <typename Callable>
    auto subscribe(Callable && callable) -> unique_subscription;

    //! Notify all compatible subjects.
    template <typename ... Arguments>
    auto notify(Arguments const & ... arguments) const -> void;

private:
    std::tuple<subject<FunctionTypes> ...> subjects_;
};

// Implementation

template<typename ... FunctionTypes>
template<typename Callable>
inline auto compound_subject<FunctionTypes ...>::subscribe(Callable && callable) -> unique_subscription
{
    using namespace detail::compound_subject;

    return apply_first<Callable>(
                [&](auto & s) {
                    return s.subscribe(std::forward<Callable>(callable));
                },
                subjects_);
}

template<typename ... FunctionTypes>
template<typename ... Arguments>
inline auto compound_subject<FunctionTypes ...>::notify(Arguments const & ... arguments) const -> void
{
    using namespace detail::compound_subject;

    apply_all<void(Arguments const & ...)>(
            [&](auto & s) {
                s.notify(arguments ...);
            },
            subjects_);
}

}
