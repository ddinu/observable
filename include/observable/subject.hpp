#pragma once
#include <memory>
#include <mutex>
#include <type_traits>
#include "observable/detail/callable_collection.hpp"
#include "observable/subscription.hpp"

namespace observable {

template <typename T>
class subject;

//! A subject stores observers and provides a way to notify them when events
//! occur.
//!
//! Observers are objects that satisfy the Callable concept and can be stored
//! inside a ``std::function<void(Args ...)>``.
//!
//! Once you call subscribe(), the observer is said to be subscribed to
//! notifications from the subject.
//!
//! Calling notify(), will call all the currently subscribed observers with the
//! arguments provided to notify().
//!
//! All methods can be safely called from multiple threads.
//!
//! \tparam Args Observer arguments. All observer types must be storable
//!              inside a ``std::function<void(Args ...)>``.
//!
//! \warning Even though subjects themselves are safe to use in parallel,
//!          observers need to handle being called from multiple threads too.
template <typename ... Args>
class subject<void(Args ...)>
{
public:
    using function_type = void(Args ...);

    //! Subscribe an observer to notifications.
    //!
    //! You can safely call this method from multiple threads.
    //!
    //! This method is reentrant, you can add and remove observers from inside
    //! other running observers.
    //!
    //! \param[in] function Observer object that will be subscribed to
    //!                     notifications from this subject.
    //!
    //! \tparam Callable Type of the observer object. This type must satisfy the
    //!                  Callable concept and must be storable inside a
    //!                  ``std::function<void(Args ...)>``.
    //!
    //! \return An unique subscription that can be used to unsubscribe the
    //!         provided observer from receiving notifications from this subject.
    //!
    //! \warning Observers must be valid and callable for as long as they are
    //!          subscribed and there is a possibility to be called.
    //!
    //!          Please keep in mind that the notify() method might still call a
    //!          subject even if it has been unsubscribed, if the unsubscribe
    //!          takes place after the notify call begins.
    //!
    //! \warning Observers must be safe to be called in parallel if the notify()
    //!          method will be called from multiple threads.
    template <typename Callable>
    auto subscribe(Callable && function) -> unique_subscription;

    //! Notify all currently subscribed observers.
    //!
    //! This method will block until all subscribed observers are called. The
    //! method will call observers one-by-one in an unspecified order.
    //!
    //! You can safely call this method in parallel from multiple threads.
    //!
    //! All observers that are subscribed at the moment of the notify() call will
    //! be called, even if some of them are removed during the call.
    //!
    //! Only observers that are subscribed at the moment of the notify() call will
    //! be called, even if new observers are added during the call.
    //!
    //! The method is reentrant, you can call notify() from inside a running
    //! observer.
    //!
    //! \param[in] arguments Arguments that will be forwarded to the subscribed
    //!                      observers.
    //!
    //! \warning All observers that will be called by a notify() call must
    //!          remain valid and callable for the duration of the notify() call.
    //!
    //! \warning If notify() is called from multiple threads, all observers must
    //!          be safe to call from multiple threads.
    auto notify(Args ... arguments) const -> void;

public:
    //! Constructor. Will create an empty subject.
    subject() = default;

    //! Subjects are **not** copy-constructible.
    subject(subject const & other) =delete;

    //! Subjects are **not** copy-assignable.
    auto operator=(subject const & other) -> subject & =delete;

    //! Subjects are move-constructible.
    subject(subject && other) noexcept =default;

    //! Subjects are move-assignable.
    auto operator=(subject && other) noexcept -> subject & =default;

private:
    using collection = detail::callable_collection<void(Args ...)>;

    std::shared_ptr<collection const> functions_ = std::make_shared<collection>();
    std::shared_ptr<std::mutex> subscribe_mutex_ = std::make_shared<std::mutex>();
};

// Implementation

template <typename ... Args>
template <typename Callable>
inline auto subject<void(Args ...)>::subscribe(Callable && function) -> unique_subscription
{
    static_assert(std::is_convertible<Callable,
                                      std::function<void(Args ...)>>::value,
                  "The provided observer object is not callable or not compatible"
                  " with the subject's function type");

    std::lock_guard<std::mutex> const subscribe_lock { *subscribe_mutex_ };

    typename collection::id id;
    {
        auto new_functions = std::make_shared<collection>(*functions_);
        id = new_functions->insert(function);
        std::atomic_store_explicit(&functions_,
                                   { new_functions },
                                   std::memory_order_relaxed);
    }

    return unique_subscription {
        [this, id, weak_subscribe_mutex = std::weak_ptr<std::mutex> { subscribe_mutex_ }]() {
            auto const subscribe_mutex = weak_subscribe_mutex.lock();
            if(!subscribe_mutex)
                return;

            std::lock_guard<std::mutex> const unsubscribe_lock { *subscribe_mutex };

            auto new_functions = std::make_shared<collection>(*functions_);
            new_functions->remove(id);
            std::atomic_store_explicit(&functions_,
                                       { new_functions },
                                       std::memory_order_relaxed);
            }
    };
}

template <typename ... Args>
inline void subject<void(Args ...)>::notify(Args ... arguments) const
{
    auto const functions = atomic_load_explicit(&functions_, std::memory_order_relaxed);
    functions->call_all(arguments ...);
}

}
