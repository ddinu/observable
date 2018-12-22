#pragma once
#include <cassert>
#include <functional>
#include <memory>
#include <type_traits>
#include <observable/detail/collection.hpp>
#include <observable/detail/type_traits.hpp>
#include <observable/subscription.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable {

//! \cond
template <typename ...>
class subject;
//! \endcond

//! Store observers and provide a way to notify them when events occur.
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
//!
//! \ingroup observable
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
    //! \warning Observers must be valid to be called for as long as they are
    //!          subscribed and there is a possibility to be called.
    //!
    //! \warning Observers must be safe to be called in parallel, if the notify()
    //!          method will be called from multiple threads.
    template <typename Callable>
    auto subscribe(Callable && observer) -> infinite_subscription
    {
        static_assert(detail::is_compatible_with_observer<Callable, observer_type>::value,
                      "The provided observer object is not callable or not compatible"
                      " with the subject");

        assert(observers_);
        auto const id = observers_->insert(observer);

        return infinite_subscription {
            [id, weak_observers = std::weak_ptr<collection> { observers_ }]() {
                auto const observers = weak_observers.lock();
                if(!observers)
                    return;

                observers->remove(id);
            }
        };
    }

    //! Subscribe an observer to notifications and immediately call it with
    //! the provided arguments.
    //!
    //! This method works exactly like the regular subscribe except it also
    //! invokes the observer.
    //!
    //! If the observer call throws an exception during the initial call, it
    //! will not be subscribed.
    //!
    //! \note The observer will not be subscribed during the initial call.
    //!
    //! \param[in] observer An observer callable that will be subscribed to
    //!                     notifications from this subject and immediately
    //!                     invoked with the provided arguments.
    //! \param[in] arguments Arguments to pass to the observer when called.
    //!
    //! \tparam Callable Type of the observer callable. This type must satisfy
    //!         the Callable concept and must be storable inside a
    //!         ``std::function<void(Args...)>``.
    //!
    //! \return An unique subscription that can be used to unsubscribe the
    //!         provided observer from receiving notifications from this
    //!         subject.
    //!
    //! \warning Observers must be valid to be called for as long as they are
    //!          subscribed and there is a possibility to be called.
    //!
    //! \warning Observers must be safe to be called in parallel, if the notify()
    //!          method will be called from multiple threads.
    //!
    //! \see subscribe()
    template <typename Callable, typename ... ActualArgs>
    auto subscribe_and_call(Callable && observer, ActualArgs ... arguments)
        -> infinite_subscription
    {
        observer(std::forward<Args ...>(arguments)...);
        return subscribe(std::forward<Callable>(observer));
    }

    //! Notify all currently subscribed observers.
    //!
    //! This method will block until all subscribed observers are called. The
    //! method will call observers one-by-one in an unspecified order.
    //!
    //! You can safely call this method in parallel, from multiple threads.
    //!
    //! \note Observers subscribed during a notify call, will not be called as
    //!       part of the notify call during which they were added.
    //!
    //! \note Observers removed during the notify call, before they themselves
    //!       have been called, will not be called.
    //!
    //! The method is reentrant; you can call notify() from inside a running
    //! observer.
    //!
    //! \param[in] arguments Arguments that will be forwarded to the subscribed
    //!                      observers.
    //!
    //! \warning All observers that will be called by notify() must remain valid
    //!          to be called for the duration of the notify() call.
    //!
    //! \warning If notify() is called from multiple threads, all observers must
    //!          be safe to call from multiple threads.
    void notify(Args ... arguments) const
    {
        assert(observers_);
        observers_->apply([&](auto && observer) { observer(arguments ...); });
    }

    //! Return true if there are no subscribers.
    auto empty() const noexcept
    {
        assert(observers_);
        return observers_->empty();
    }

public:
    //! Constructor. Will create an empty subject.
    subject() =default;

    //! Subjects are **not** copy-constructible.
    subject(subject const &) =delete;

    //! Subjects are **not** copy-assignable.
    auto operator=(subject const &) -> subject & =delete;

    //! Subjects are move-constructible.
    subject(subject &&) noexcept =default;

    //! Subjects are move-assignable.
    auto operator=(subject &&) noexcept -> subject & =default;

private:
    using collection = detail::collection<std::function<observer_type>>;

    std::shared_ptr<collection> observers_ { std::make_shared<collection>() };
};

//! Subject specialization that can be used inside a class, as a member, to
//! prevent external code from calling notify(), but still allow anyone to
//! subscribe.
//!
//! \note Except for the notify() method being private, this specialization is
//!       exactly the same as a regular subject.
//!
//! \tparam ObserverType The function type of the observers that will subscribe
//!                      to notifications.
//!
//! \tparam EnclosingType This type will be declared a friend of the subject and
//!                       will have access to the notify() method.
//!
//! \see subject<void(Args ...)>
//! \ingroup observable
template <typename ObserverType, typename EnclosingType>
class subject<ObserverType, EnclosingType> : public subject<ObserverType>
{
public:
    using subject<ObserverType>::subject;

private:
    //! \see subject<void(Args...)>::notify
    using subject<ObserverType>::notify;

    friend EnclosingType;
};

}

OBSERVABLE_END_CONFIGURE_WARNINGS
