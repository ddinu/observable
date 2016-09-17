#pragma once
#include <memory>
#include <mutex>
#include <type_traits>
#include "observable/detail/callable_collection.hpp"
#include "observable/detail/subscription.hpp"

namespace observable {

using unique_subscription = detail::unique_subscription;
using shared_subscription = detail::shared_subscription;

//! This class stores observers and provides a way to notify them.
//!
//! An observer is any object that satisfies the Callable concept and can be
//! stored inside a ``std::function<FunctionType>``.s
//!
//! Once you call subscribe(), the observer is said to be subscribed to
//! notifications from the subject.
//!
//! A call to notify(), calls all subscribed observers that have been subscribed.
//!
//! \tparam FunctionType All observer types must be storable by a
//!                      std::function<FunctionType>.
//! \note All methods defined in this class can be safely called in parallel.
template <typename FunctionType>
class subject
{
    static_assert(std::is_function<FunctionType>::value,
                  "FunctionType is not defining a function.");

    static_assert(std::is_same<
                        typename std::function<FunctionType>::result_type,
                        void
                   >::value,
                  "FunctionType must not return a value.");

public:
    //! Function type of the subject.
    using function_type = FunctionType;

    //! Subscribe to notifications.
    //!
    //! The Callable type must satisfy the Callable concept and must be storable
    //! inside a std::function<FunctionType>.
    template <typename Callable>
    auto subscribe(Callable && function) -> unique_subscription;

    //! Notify all observers.
    template <typename ... Arguments>
    auto notify(Arguments && ... arguments) const -> void;

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
    auto operator=(subject && other) noexcept -> subject & =default;

private:
    using collection = detail::callable_collection<FunctionType>;

    std::shared_ptr<collection> functions_ = std::make_shared<collection>();
    mutable std::shared_ptr<std::mutex> mutex_ = std::make_shared<std::mutex>();
};

// Implementation

template <typename FunctionType>
template <typename Callable>
inline auto subject<FunctionType>::subscribe(Callable && function) -> unique_subscription
{
    static_assert(std::is_convertible<Callable,
                                      std::function<FunctionType>>::value,
                  "The provided observer object is not callable or not compatible"
                  " with the subject's function type");

    std::lock_guard<std::mutex> subscribe_lock { *mutex_ };
    functions_ = std::make_shared<collection>(*functions_);

    auto id = functions_->insert(function);

    return unique_subscription {
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

template <typename FunctionType>
template<typename ... Arguments>
inline void subject<FunctionType>::notify(Arguments && ... arguments) const
{
    static_assert(std::is_convertible<void(Arguments && ...),
                                      std::function<FunctionType>>::value,
                 "The provided arguments are not compatible with the subject's "
                 "function type.");

    auto functions = functions_;
    functions->call_all(std::forward<Arguments>(arguments) ...);
}

}
