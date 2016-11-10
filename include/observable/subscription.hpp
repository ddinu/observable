#pragma once
#include <atomic>
#include <functional>
#include <memory>

namespace observable {

//! Unique subscription that will unsubscribe the associated observer when it
//! is destroyed.
//!
//! This class is movable but not copyable.
//!
//! \note All methods of this class can be safely called in parallel.
class unique_subscription final
{
public:
    //! This class is default-constructible.
    unique_subscription() = default;

    //! Create an unique subscription with the specified unsubscribe function.
    explicit unique_subscription(std::function<void()> const & unsubscriber) :
        unsubscribe_ { unsubscriber }
    {
    }

    //! Call the associated unsubscribe function to unsubscribe the associated
    //! observer.
    //!
    //! Only the first call of this method will have an effect.
    //!
    //! \note If release() has been called, this method will have no effect.
    auto unsubscribe() &
    {
        if(!called_->test_and_set())
        {
            try {
                unsubscribe_();
            } catch(...) {
                called_.reset();
                throw;
            }
        }
    }

    //! Disassociate the subscription from the stored unsubscribe function and
    //! return the unsubscribe function without calling it.
    //!
    //! After calling this method, unsubscribe() or destroying the subscription
    //! instance will have no effect.
    auto release() &&
    {
        using std::swap;
        decltype(unsubscribe_) unsub = []() { };
        swap(unsub, unsubscribe_);

        return unsub;
    }

    //! Destructor. Will call unsubscribe().
    //!
    //! \note If release() has been called, this method will have no effect.
    ~unique_subscription()
    {
        if(called_ && unsubscribe_)
            unsubscribe();
    }

    //! This class is not copy-constructible.
    unique_subscription(unique_subscription const & ) = delete;

    //! This class is not copy-assignable.
    unique_subscription & operator=(unique_subscription const &) = delete;

    //! This class is move-constructible.
    unique_subscription(unique_subscription &&) = default;

    //! This class is move-assignable.
    unique_subscription & operator=(unique_subscription &&) = default;

private:
    std::function<void()> unsubscribe_ = []() { };

    // std::call_once with a std::once_flag would have worked, but it requires
    // pthreads on Linux. We're using this in order not to bring in that
    // dependency.
    std::unique_ptr<std::atomic_flag> called_ = std::make_unique<std::atomic_flag>();
};

//! Shared subscription that will unsubscribe the associated observer when the
//! last instance of the class is destroyed.
//!
//! This class is movable and copyable.
//!
//! \note All methods of this class can be safely called in parallel.
class shared_subscription final
{
public:
    //! Create a shared subscription from an unique one.
    //!
    //! The unique subscription will be released.
    shared_subscription(unique_subscription && subscription) :
        unsubscribe_ { std::make_shared<unique_subscription>(std::move(subscription)) }
    {
    }

    //! Create an invalid shared handle.
    //!
    //! Calling unsubscribe on this handle will have no effect.
    shared_subscription() = default;

    //! Manually call unsubscribe.
    auto unsubscribe() &
    {
        unsubscribe_.reset();
    }

    //! Check if the handle is valid.
    explicit operator bool() const noexcept
    {
        return !!unsubscribe_;
    }

private:
    std::shared_ptr<unique_subscription> unsubscribe_;
};

}
