#pragma once
#include <memory>
#include "../function_collection.hpp"
#include "../function_handle.hpp"

namespace observable { namespace detail { namespace subject {

//! Handle to manage a subscription.
class subscription
{
public:
    subscription() = default;

    //! Unsubscribe from all notifications.
    void unsubscribe()
    {
        if(handle)
            handle->remove();
    }

    explicit operator bool() const
    {
        return !!handle;
    }

private:
    //! Create a new subscription.
    explicit subscription(function_collection::function_id const & id) :
        handle(std::make_shared<function_handle>(id))
    {
    }

    std::shared_ptr<function_handle> handle;

    template <typename, typename>
    friend class data;
};

class auto_unsubscribe : private subscription
{
public:
    auto_unsubscribe() =default;

    auto_unsubscribe(subscription const & sub) :
        subscription(sub)
    {
        unsubscriber_.reset(this, [this](void *) {
            unsubscribe();
        });
    }

    void unsubscribe()
    {
        subscription::unsubscribe();
    }

    using subscription::operator bool;

private:
    std::shared_ptr<void> unsubscriber_;
};

} } }
