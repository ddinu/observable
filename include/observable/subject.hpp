#pragma once
#include "detail/subject/copy_policy.hpp"
#include "detail/subject/data.hpp"
#include "detail/subject/subscription.hpp"
#include "detail/function_collection.hpp"
#include "detail/function_traits.hpp"

namespace observable {

//! Handle that manages subscription.
using subscription = detail::subject::subscription;
using auto_unsubscribe = detail::subject::auto_unsubscribe;

namespace copy {
    struct subject_copy_policy_ { };
    namespace s_ = detail::subject;

    //! The subject will not be copyable.
    using none = s_::no_copy<subject_copy_policy_>;

    //! The subject will be copyable but all subscriptions and internal data
    //! will be shared.
    using shallow = s_::shallow_copy<subject_copy_policy_>;

    //! The subject will be copyable but the copy will not have any subscriptions.
    using deep = s_::deep_copy<subject_copy_policy_>;
}

//! This class stores observers and provides a way to notify them with
//! heterogeneous parameters.
//!
//! An observer is a generic function. Once you call subscribe(), the observer
//! is said to be subscribed to notifications from the subject.
//!
//! A call to notify() or notify_tagged() calls all observers that have matching
//! parameter types and have subscribed with the same tag value (in case you
//! called notify_tagged).
//!
//! \tparam Tag The type of the tag parameter for tagged subscriptions.
//! \note All methods defined in this class are safe to be called in parallel.
template <typename Tag=std::string, typename CopyPolicy=copy::none>
class subject : public CopyPolicy,
                detail::subject::data<Tag, CopyPolicy>
{
public:
    //! Subscribe to all untagged notifications.
    template <typename Function>
    subscription subscribe(Function && function);

    //! Subscribe to all notifications tagged with the provided tag value.
    template <typename T, typename Function>
    subscription subscribe(T && tag, Function && fun);

    //! Notify all untagged subscriptions.
    template <typename ... Arguments>
    void notify(Arguments ... arguments) const;

    //! Notify all tagged subscriptions that have used the provided tag value
    //! when subscribing.
    template <typename T, typename ... Arguments>
    void notify_tagged(T && tag, Arguments ... arguments) const;
};

namespace detail {
    //! Check if the provided function can be used as a subscription callback
    //! function.
    template <typename Function>
    constexpr auto check_compatibility()
    {
        using traits = function_traits<Function>;

        static_assert(std::is_same<typename traits::return_type, void>::value,
                      "Subscription function cannot return a value.");

        static_assert(std::is_convertible<
                            std::function<typename traits::signature>,
                            std::function<typename traits::normalized>>::value,
                      "Subscription function arguments must not be non-const "
                      "references.");
    }

    //! Call the provided collection with the provided arguments.
    template <typename Collection, typename ... Arguments>
    auto call(Collection && collection, Arguments && ... arguments)
    {
        using signature = typename function_traits<void(Arguments ...)>::normalized;
        check_compatibility<signature>(); // This should never fail.

        collection.template call_all<signature>(std::forward<Arguments>(arguments) ...);
    }
}

template <typename Tag, typename CopyPolicy>
template <typename Function>
inline auto subject<Tag, CopyPolicy>::subscribe(Function && function) -> subscription
{
    using traits = detail::function_traits<Function>;
    using signature = typename traits::normalized;
    detail::check_compatibility<Function>();

    typename subject::lock_guard lock { subject::data_mutex() };

    auto * collection = subject::untagged();
    assert(collection);

    auto id = collection->template insert<signature>(std::forward<Function>(function));
    return subject::make_subscription(subject::untagged(), id);
}

template <typename Tag, typename CopyPolicy>
template <typename T, typename Function>
inline auto subject<Tag, CopyPolicy>::subscribe(T && tag, Function && function) -> subscription
{
    using traits = detail::function_traits<Function>;
    using signature = typename traits::normalized;
    detail::check_compatibility<Function>();

    typename subject::lock_guard lock { subject::data_mutex() };

    auto * collection = subject::tagged(tag);
    assert(collection);

    auto id = collection->template insert<signature>(std::forward<Function>(function));
    return subject::make_subscription(collection, id);
}

template <typename Tag, typename CopyPolicy>
template<typename ... Arguments>
inline void subject<Tag, CopyPolicy>::notify(Arguments ... arguments) const
{
    detail::function_collection snapshot;

    {
        typename subject::lock_guard lock { subject::data_mutex() };
        snapshot = *subject::untagged();
    }

    detail::call(snapshot, std::forward<Arguments>(arguments) ...);
}

template <typename Tag, typename CopyPolicy>
template<typename T, typename ... Arguments>
inline void subject<Tag, CopyPolicy>::notify_tagged(T && tag, Arguments ... arguments) const
{
    detail::function_collection snapshot;

    {
        typename subject::lock_guard lock { subject::data_mutex() };
        snapshot = *subject::tagged(tag);
    }

    detail::call(snapshot, std::forward<Arguments>(arguments) ...);
}

}
