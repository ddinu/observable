#pragma once
#include <atomic>
#include <cassert>
#include <functional>
#include <memory>
#include <mutex>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable {

//! Infinite subscription that will not unsubscribe the associated observer
//! when destroyed.
//!
//! \ingroup observable
class infinite_subscription
{
public:
    //! Create a subscription with the specified unsubscribe functor.
    //!
    //! \param[in] unsubscribe Calling this functor will unsubscribe the
    //!                        associated observer.
    //! \note This is for internal use by subject instances.
    explicit infinite_subscription(std::function<void()> const & unsubscribe) :
        unsubscribe_ { unsubscribe }
    { }

    //! Unsubscribe the associated observer from receiving notifications.
    //!
    //! Only the first call of this method will have an effect.
    //!
    //! \note If release() has been called, this method will have no effect.
    void unsubscribe()
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

public:
    //! This class is default-constructible.
    infinite_subscription() =default;

    //! This class is not copy-constructible.
    infinite_subscription(infinite_subscription const & ) =delete;

    //! This class is not copy-assignable.
    auto operator=(infinite_subscription const &) -> infinite_subscription & =delete;

    //! This class is move-constructible.
    infinite_subscription(infinite_subscription &&) = default;

    //! This class is move-assignable.
    auto operator=(infinite_subscription &&) -> infinite_subscription & =default;

private:
    std::function<void()> unsubscribe_ { []() { } };

    // std::call_once with a std::once_flag would have worked, but it requires
    // pthreads on Linux. We're using this in order not to bring in that
    // dependency.
    std::unique_ptr<std::atomic_flag> called_ { std::make_unique<std::atomic_flag>() };
};

//! Unsubscribe the associated observer when destroyed.
//!
//! \note All methods of this class can be safely called in parallel, from multiple
//!       threads.
//!
//! \ingroup observable
class unique_subscription final : public infinite_subscription
{
public:
    //! Create an empty subscription.
    //!
    //! \note Calling unsubscribe on an empty subscription will have no effect.
    unique_subscription() =default;

    //! Create an unique subscription from an infinite_subscription.
    //!
    //! \param[in] sub An infinite subscription that will be converted to an
    //!                unique_subscription.
    unique_subscription(infinite_subscription && sub) :
        infinite_subscription(std::move(sub))
    { }

    //! Destructor. Will call unsubscribe().
    //!
    //! \note If release() has been called, this will have no effect.
    ~unique_subscription() { unsubscribe(); }

    using infinite_subscription::operator=;

public:
    //! This class is not copy-constructible.
    unique_subscription(unique_subscription const & ) =delete;

    //! This class is not copy-assignable.
    auto operator=(unique_subscription const &) -> unique_subscription & =delete;

    //! This class is move-constructible.
    unique_subscription(unique_subscription &&) = default;

    //! This class is move-assignable.
    auto operator=(unique_subscription &&) -> unique_subscription & =default;
};

//! Unsubscribe the associated observer when the last instance of the class is
//! destroyed.
//!
//! \note All methods of this class can be safely called in parallel, from multiple
//!       threads.
//!
//! \ingroup observable
class shared_subscription final
{
public:
    //! Create a shared subscription from a temporary infinite subscription.
    //!
    //! \param subscription An infinite subscription that will be converted to a
    //!                     shared subscription.
    //! \note The unique subscription will be released.
    explicit shared_subscription(infinite_subscription && subscription) :
        unsubscribe_ { std::make_shared<unique_subscription>(std::move(subscription)) },
        mut_ { std::make_shared<std::mutex>() }
    {
    }

    //! Create an empty shared subscription.
    //!
    //! Calling unsubscribe on an empty shared subscription will have no effect.
    shared_subscription() noexcept =default;

    //! Unsubscribe the associated observer from receiving notifications.
    //!
    //! Only the first call of this method will have an effect.
    void unsubscribe()
    {
        if(!mut_)
        {
            assert(!unsubscribe_);
            return;
        }

        std::lock_guard<std::mutex> lock { *mut_ };
        unsubscribe_.reset();
    }

    //! Return true if the subscription is not empty.
    explicit operator bool() const noexcept { return !!unsubscribe_; }

public:
    //! This class is copy-constructible.
    shared_subscription(shared_subscription const & ) =default;

    //! This class is copy-assignable.
    auto operator=(shared_subscription const &) -> shared_subscription & =default;

    //! This class is move-constructible.
    shared_subscription(shared_subscription &&) = default;

    //! This class is move-assignable.
    auto operator=(shared_subscription &&) -> shared_subscription & =default;

private:
    std::shared_ptr<unique_subscription> unsubscribe_;
    std::shared_ptr<std::mutex> mut_;
};

}

OBSERVABLE_END_CONFIGURE_WARNINGS
