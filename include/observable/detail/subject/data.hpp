#pragma once
#include <algorithm>
#include <cassert>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "copy_policy.hpp"
#include "subscription.hpp"
#include "observable/detail/function_collection.hpp"
#include "observable/detail/function_handle.hpp"

namespace observable { namespace detail { namespace subject {

using mutex_ = std::mutex;
using lock_guard_ = std::lock_guard<mutex_>;
struct data_copy_policy_ { };

template <>
struct no_copy<data_copy_policy_> : no_copy<>
{
protected:
    template <typename T>
    using ptr_type = std::unique_ptr<T>;

    template <typename data>
    void copy_impl(data const & source, data & target)
    {
        assert(!"Not implemented");
    }
};

template <>
struct shallow_copy<data_copy_policy_> : shallow_copy<>
{
protected:
    template <typename T>
    using ptr_type = std::shared_ptr<T>;

    template <typename data>
    void copy_impl(data const & source, data & target)
    {
        lock_guard_ this_lock { *target.data_mutex_ };
        lock_guard_ other_lock { *source.data_mutex_ };

        target.untagged_ = source.untagged_;
        target.tagged_ = source.tagged_;
        target.handles_ = source.handles_;
    }
};

template <>
struct deep_copy<data_copy_policy_> : deep_copy<>
{
protected:
    template <typename T>
    using ptr_type = std::unique_ptr<T>;

    template <typename data>
    void copy_impl(data const & source, data & target)
    {
        lock_guard_ this_lock { *target.data_mutex_ };
        lock_guard_ other_lock { *source.data_mutex_ };

        target.untagged_.reset(new function_collection { *source.untagged_ });

        for(auto && kv : source.tagged_)
            target.tagged_[kv.first].reset(new function_collection { *kv.second });

        target.handles_ = source.handles_;
        target.purge_dead_handles();

        for(auto && p : target.handles_)
        {
            auto h = p.lock();
            if(!h)
                continue;

            if(target.untagged_->contains(h->function_id()))
                h->register_collection(target.untagged_.get());

            for(auto && kv : target.tagged_)
                if(kv.second->contains(h->function_id()))
                    h->register_collection(kv.second.get());
        }
    }
};

template <typename Tag, typename CopyPolicy>
class data : public CopyPolicy::template rebind<data_copy_policy_>
{
public:
    using mutex = mutex_;
    using lock_guard = lock_guard_;

    auto untagged() const { return untagged_.get(); }
    auto untagged() { return untagged_.get(); }

    mutex & data_mutex() { return *data_mutex_; }
    mutex & data_mutex() const { return *data_mutex_; }

    template <typename T>
    auto tagged(T && tag) const
    {
        auto it = tagged_.find(tag);
        assert(it != end(tagged_));
        return it != end(tagged_) ? it->second.get() : nullptr;
    }

    template <typename T>
    auto tagged(T && tag)
    {
        auto && collection = tagged_[tag];
        if(!collection)
            collection.reset(new function_collection { });

        return collection.get();
    }

    auto make_subscription(detail::function_collection * collection,
                           detail::function_collection::function_id const & id)
    {
        purge_dead_handles();

        subscription sub { id };

        sub.handle->register_collection(collection);
        handles_.push_back(sub.handle);

        return sub;
    }

    auto purge_dead_handles()
    {
        auto r = std::remove_if(begin(handles_),
                                end(handles_),
                                [](auto && h) { return h.expired(); });
        handles_.erase(r, end(handles_));
    }

    ~data()
    {
        purge_dead_handles();

        // Unregister our collections from all subscriptions.
        for(auto && p : handles_)
        {
            auto h = p.lock();
            if(!h)
                continue;

            h->unregister_collection(untagged_.get());

            for(auto && kv : tagged_)
                h->unregister_collection(kv.second.get());
        }
    }

    data() =default;

    data(data const & other)
    {
        data::copy_impl(other, *this);
    }

    data(data &&) =default;

    data & operator=(data other)
    {
        swap(*this, other);
        return *this;
    }

    friend void swap(data & a, data & b)
    {
        using std::swap;
        lock_guard a_lock { *a.data_mutex_ };
        lock_guard b_lock { *b.data_mutex_ };

        swap(a.tagged_, b.tagged_);
        swap(a.untagged_, b.untagged_);
        swap(a.handles_, b.handles_);
    }

private:
    template <typename T>
    using ptr = typename data::template ptr_type<T>; // Fix for MSVC

    mutable ptr<mutex> data_mutex_ = ptr<mutex> { new mutex { } };

    std::unordered_map<Tag, ptr<function_collection>> tagged_;

    ptr<function_collection> untagged_ =
                ptr<function_collection> { new function_collection { } };

    std::vector<std::weak_ptr<function_handle>> handles_;

    template <typename> friend struct no_copy;
    template <typename> friend struct shallow_copy;
    template <typename> friend struct deep_copy;
};

} } }
