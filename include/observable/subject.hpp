#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include "detail/function_collection.hpp"
#include "detail/function_traits.hpp"
#include "detail/handle.hpp"
#include "detail/tagged_function.hpp"

namespace observable {

//! Handle that manages a subscription.
using subscription = detail::handle;
using auto_unsubscribe = detail::auto_handle;

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
template <typename Tag=std::string>
class subject
{
public:
    //! Subscribe to all untagged notifications.
    template <typename Function>
    auto subscribe(Function && function) -> subscription;

    //! Subscribe to all notifications tagged with the provided tag value.
    template <typename T, typename Function>
    auto subscribe(T && tag, Function && fun) -> subscription;

    //! Notify all untagged subscriptions.
    template <typename ... Arguments>
    auto notify_untagged(Arguments && ... arguments) const -> void;

    //! Notify all tagged subscriptions that have used the provided tag value
    //! when subscribing.
    template <typename T, typename ... Arguments>
    auto notify_tagged(T && tag, Arguments && ... arguments) const -> void;

public:
    //! Create an empty subject.
    subject() = default;

    //! Subject is not copy-constructible.
    subject(subject const & other) =delete;

    //! Subject is not copy-assignable.
    auto operator=(subject const & other) -> subject & =delete;

    //! Subject is move-constructible.
    subject(subject && other) noexcept =default;

    //! Subject is move-assignable.
    auto operator=(subject && other) -> subject & =default;

private:
    using collection = detail::function_collection;

    std::shared_ptr<collection> functions_ { std::make_shared<collection>() };
    mutable std::shared_ptr<std::mutex> mutex_ { std::make_shared<std::mutex>() };
};

namespace detail {
    //! Choose the type that will be used to tag functions.
    template <typename ArgTag, typename ClassTag>
    using actual_tag = typename std::conditional<
                                    std::is_same<
                                        typename std::remove_cv<
                                            typename std::remove_reference<ArgTag>::type
                                        >::type,
                                        no_tag>::value,
                                    no_tag,
                                    ClassTag>::type;

    //! The type that a subscription function will have inside function
    //! collections.
    template <typename Function>
    using actual_function_type = typename const_ref_args<
                                    typename function_traits<Function>::type
                                 >::type;

    //! Check if the provided function can be used as a subscription callback
    //! function.
    template <typename Function>
    constexpr auto check_compatibility()
    {
        using traits = function_traits<Function>;

        static_assert(std::is_same<typename traits::return_type, void>::value,
                      "Subscription function cannot return a value.");

        static_assert(std::is_convertible<
                            std::function<typename traits::type>,
                            std::function<actual_function_type<Function>>>::value,
                      "Subscription function arguments must not be non-const "
                      "references.");
    }
}

template <typename Tag>
template <typename Function>
inline auto subject<Tag>::subscribe(Function && function) -> subscription
{
    return subscribe(detail::no_tag { }, std::forward<Function>(function));
}

template <typename Tag>
template <typename T, typename Function>
inline auto subject<Tag>::subscribe(T && tag, Function && function) -> subscription
{
    namespace d = detail;
    d::check_compatibility<Function>();

    using actual_tag = d::actual_tag<T, Tag>;
    using tagged_function = d::tagged<actual_tag, Function>;
    using actual_function_type = d::actual_function_type<
                                    typename tagged_function::type>;

    std::lock_guard<std::mutex> subscribe_lock { *mutex_ };
    functions_ = std::make_shared<collection>(*functions_);

    auto id = functions_->insert<actual_function_type>(
                                             tagged_function {
                                                actual_tag { tag },
                                                std::forward<Function>(function)
                                             });

    return subscription {
                [=, mutex = std::weak_ptr<std::mutex> { mutex_ }]() mutable {
                    auto mutex_ptr = mutex.lock();
                    if(!mutex_ptr)
                        return;

                    std::lock_guard<std::mutex> unsubscribe_lock { *mutex_ptr };
                    functions_ = std::make_shared<collection>(*functions_);

                    functions_->remove(id);
                }
            };
}

template <typename Tag>
template<typename ... Arguments>
inline void subject<Tag>::notify_untagged(Arguments && ... arguments) const
{
    return notify_tagged(detail::no_tag { },
                         std::forward<Arguments>(arguments) ...);
}

template <typename Tag>
template<typename T, typename ... Arguments>
inline void subject<Tag>::notify_tagged(T && tag, Arguments && ... arguments) const
{
    namespace d = detail;
    using function_type = void(*)(Arguments ...);
    d::check_compatibility<function_type>();

    using actual_tag = d::actual_tag<T, Tag>;
    using tagged_function = d::tagged<actual_tag, function_type>;
    using actual_function_type = d::actual_function_type<
                                    typename tagged_function::type>;

    auto functions = functions_;
    functions->template call_all<actual_function_type>(
                                        actual_tag { tag },
                                        std::forward<Arguments>(arguments) ...);
}

}
