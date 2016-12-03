#pragma once
#include <functional>
#include <memory>
#include <type_traits>
#include "observable/detail/collection.hpp"
#include "observable/detail/type_traits.hpp"
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
//! All methods can be safely called in parallel, from multiple threads.
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
    using observer_type = void(Args ...);

    //! Subscribe an observer to notifications.
    //!
    //! You can safely call this method in parallel, from multiple threads.
    //!
    //! This method is reentrant, you can add and remove observers from inside
    //! other, running, observers.
    //!
    //! \param[in] observer An observer callable that will be subscribed to
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
    //! \warning Observers must be safe to be called in parallel if the notify()
    //!          method will be called from multiple threads.
    template <typename Callable>
    auto subscribe(Callable && observer) -> unique_subscription;

    //! Notify all currently subscribed observers.
    //!
    //! This method will block until all subscribed observers are called. The
    //! method will call observers one-by-one in an unspecified order.
    //!
    //! You can safely call this method in parallel, from multiple threads.
    //!
    //! \note Observers subscribed during a notify call will not be called as
    //!       part of the notify call during which they were added.
    //!
    //! \note Observers removed during the notify call, before they themselves
    //!       have been called, will not be called.
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
    using collection = detail::collection<std::function<observer_type>>;

    std::shared_ptr<collection> observers_ { std::make_shared<collection>() };
};

// Implementation

template <typename ... Args>
template <typename Callable>
inline auto subject<void(Args ...)>::subscribe(Callable && observer) -> unique_subscription
{
    static_assert(detail::is_compatible_with_observer<Callable, observer_type>::value,
                  "The provided observer object is not callable or not compatible"
                  " with the subject");

    auto id = observers_->insert(observer);

    return unique_subscription {
        [this, id, weak_observers = std::weak_ptr<collection> { observers_ }]() {
            auto const observers = weak_observers.lock();
            if(!observers)
                return;

            observers->remove(id);
        }
    };
}

template <typename ... Args>
inline void subject<void(Args ...)>::notify(Args ... arguments) const
{
    observers_->apply([&](auto && observer) { observer(arguments ...); });
}

}
