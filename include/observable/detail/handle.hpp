#pragma once
#include <memory>
#include <functional>
#include <mutex>

namespace observable { namespace detail {

//! Store a function and call it when unsubscribe() is called.
class handle
{
public:
    //! Create a new handle.
    explicit handle(std::function<void()> const & unsubscriber) :
        unsubscribe_(unsubscriber)
    {
    }

    //! Call the associated unsubscribe function.
    auto unsubscribe()
    {
        std::call_once(*once_, unsubscribe_);
    }

private:
    std::function<void()> unsubscribe_;
    std::shared_ptr<std::once_flag> once_ { std::make_shared<std::once_flag>() };
};

//! Handle that automatically calls unsubscribe() when the last instance is
//! destroyed.
class auto_handle
{
public:
    //! Create a new handle.
    auto_handle(handle const & h) :
        handle_(new handle(h), [](auto * hp) { hp->unsubscribe(); delete hp; })
    {
    }

    //! Create an invalid handle.
    auto_handle() =default;

    //! Manually call unsubscribe.
    auto unsubscribe()
    {
        handle_->unsubscribe();
    }

    //! Check if the handle is valid.
    explicit operator bool() const
    {
        return !!handle_;
    }

private:
    std::shared_ptr<handle> handle_;
};

} }
