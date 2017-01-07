#pragma once
#include <atomic>
#include <functional>
#include <memory>

namespace observable {

//! Unique subscriptions will unsubscribe the associated observer when they
//! are destroyed.
//!
//! This class is movable but not copyable.
//!
//! \note All methods of this class can be safely called in parallel from multiple
//!       threads.
class unique_subscription final
{
public:
    //! Create an empty subscription.
    //!
    //! Calling unsubscribe on an empty subscription will have no effect.
    unique_subscription() =default;

    //! Create a subscription with the specified unsubscribe functor.
    //!
    //! \param[in] unsubscribe This functor will be called when the unique
    //!                        subscription goes out of scope or when
    //!                        unsubscribe() has been called.
    //! \internal
    explicit unique_subscription(std::function<void()> const & unsubscribe) :
        unsubscribe_ { unsubscribe }
    {
    }

    //! Unsubscribe the associated observer from receiving notifications.
    //!
    //! Only the first call of this method will have an effect.
    //!
    //! \note If release() has been called, this method will have no effect.
    auto unsubscribe()
    {
        if(!called_ || !unsubscribe_ || called_->test_and_set())
            return;

        try {
            if(unsubscribe_) unsubscribe_();
        } catch(...) {
            called_->clear();
            throw;
        }
    }

    //! Disassociate the subscription from the subscribed observer.
    //!
    //! After calling this method, calling unsubscribe() or destroying the
    //! subscription instance will have no effect.
    //!
    //! \return Functor taking no parameters that will perform the unsubscribe
    //!         when called.
    //!         For example: ``subscription.release()()`` is equivalent to
    //!         ``subscription.unsubscribe()``.
    auto release()
    {
        using std::swap;
        decltype(unsubscribe_) unsub = []() { };
        swap(unsub, unsubscribe_);

        return unsub;
    }

    //! Destructor. Will call unsubscribe().
    //!
    //! \note If release() has been called, this will have no effect.
    ~unique_subscription()
    {
        unsubscribe();
    }

public:
    //! This class is not copy-constructible.
    unique_subscription(unique_subscription const & ) =delete;

    //! This class is not copy-assignable.
    auto operator=(unique_subscription const &) -> unique_subscription & =delete;

    //! This class is move-constructible.
    unique_subscription(unique_subscription &&) = default;

    //! This class is move-assignable.
    auto operator=(unique_subscription &&) -> unique_subscription & =default;

private:
    std::function<void()> unsubscribe_ { []() { } };

    // std::call_once with a std::once_flag would have worked, but it requires
    // pthreads on Linux. We're using this in order not to bring in that
    // dependency.
    std::unique_ptr<std::atomic_flag> called_ { std::make_unique<std::atomic_flag>() };
};

//! Shared subscriptions will unsubscribe the associated observer when the last
//! instance of the class is destroyed.
//!
//! This class is both movable and copyable.
//!
//! \note All methods of this class can be safely called in parallel from multiple
//!       threads.
class shared_subscription final
{
public:
    //! Create a shared subscription from a r-value unique subscription.
    //!
    //! \note The unique subscription will be released.
    explicit shared_subscription(unique_subscription && subscription) :
        unsubscribe_ { std::make_shared<unique_subscription>(std::move(subscription)) }
    {
    }

    //! Create an empty shared subscription.
    //!
    //! Calling unsubscribe on an empty shared subscription will have no effect.
    shared_subscription() noexcept =default;

    //! Unsubscribe the associated observer from receiving notifications.
    //!
    //! Only the first call of this method will have an effect.
    auto unsubscribe()
    {
        unsubscribe_.reset();
    }

    //! Return true if the subscription is not empty.
    explicit operator bool() const noexcept
    {
        return !!unsubscribe_;
    }

private:
    std::shared_ptr<unique_subscription> unsubscribe_;
};

}
