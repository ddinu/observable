#pragma once
#include <atomic>
#include <cassert>

namespace observable { namespace detail {

//! Thread-safe collection that can store items and can apply a functor over them.
//!
//! All methods of the collection can be safely called in parallel, from multiple
//! threads.
//!
//! \note The order of elements inside the collection is unspecified.
//!
//! \tparam ValueType Type of the elements that will be stored inside the
//!                   collection. This type must be at least move constructible.
//! \internal
template <typename ValueType>
class collection final
{
public:
    //! Identifier for an element that has been inserted. You can use this to
    //! remove a previously inserted object.
    using id = std::size_t;

    //! Insert a new element into the collection.
    //!
    //! \param element The object to be inserted.
    //! \tparam ValueType_ Type of the inserted element. Must be convertible to
    //!                    the collection's ValueType.
    //!
    //! \return An id that can be used to remove the inserted element.
    //!         This id is stable, meaning that you can use it even after mutating
    //!         the collection.
    //!
    //! \note Any apply() call running concurrently with an insert(), that has
    //!       already called its functor for at least one element, is guaranteed
    //!       to not call the functor for this newly inserted element.
    template <typename ValueType_>
    auto insert(ValueType_ && element)
    {
        auto const i = ++last_id_;
        auto n = std::make_unique<node>();
        n->node_id = i;
        n->element = std::forward<ValueType_>(element);

        {
            gc_blocker const block { this };

            n->next = head_.load();
            while(!head_.compare_exchange_weak(n->next, n.get()))
                ;

            n.release();
        }

        gc();
        return i;
    }

    //! Remove a previously inserted element from the collection.
    //!
    //! If no element with the provided id exists, this method does nothing.
    //!
    //! \param element_id Id returned by insert.
    //!
    //! \return True if an element of the collection was removed, false if no
    //!         element has been removed.
    //!
    //! \note Any apply() call running concurrently with a remove() call,
    //!       that has not already called its functor with the removed element,
    //!       is guaranteed to not call the functor with the removed element.
    auto remove(id const & element_id)
    {
        auto deleted = false;
        {
            gc_blocker const block { this };

            for(auto n = head_.load(); n; n = n->next)
            {
                if(n->node_id != element_id)
                    continue;

                deleted = !n->deleted.exchange(true);
                break;
            }
        }

        gc();
        return deleted;
    }

    //! Apply an unary functor over all elements of the collection.
    //!
    //! The functor will be called with each element, in an unspecified order.
    //!
    //! \note This method is reentrant, meaning you can call insert and remove
    //!       on the same collection from inside this functor.
    //!
    //! \note It is well defined and supported to remove() the element currently
    //!       passed to the functor, even before the functor returns.
    //!
    //! \param fun A functor that will be called with each element of the
    //!            collection.
    template <typename UnaryFunctor>
    auto apply(UnaryFunctor && fun) const
    {
        gc_blocker const block { this };

        for(auto n = head_.load(); n; n = n->next)
        {
            if(n->deleted.load())
                continue;

            fun(n->element);
        }
    }

    //! Check if the collection is empty.
    auto empty() const
    {
        gc_blocker const block { this };
        auto const h = head_.load();
        return !h || h->deleted;
    }

    //! Destructor.
    ~collection()
    {
        for(auto n = head_.load(); n; n = n->next)
            n->deleted.store(true);

        gc();
    }

private:
    //! Delete any nodes marked as deleted.
    void gc()
    {
        if(block_gc_.load() > 0 || gc_active_.exchange(true))
            return;

        auto head = head_.load();
        for(auto p = &head; *p && block_gc_ == 0;)
        {
            if(!(*p)->deleted.load())
            {
                p = &((*p)->next);
                continue;
            }

            auto d = *p;
            *p = (*p)->next;
            delete d;

            if(p == &head)
                head_.store(*p);
        }

        gc_active_.store(false);
    }

    //! Block the gc() method from running for the duration of an instance's
    //! lifetime.
    struct gc_blocker
    {
        gc_blocker(collection<ValueType> const * c) : collection_(c)
        {
            ++collection_->block_gc_;
            while(collection_->gc_active_.load())
                ;
        }

        ~gc_blocker()
        {
            --collection_->block_gc_;
        }

    private:
        collection<ValueType> const * collection_;
    };

    //! Node data.
    struct node
    {
        node * next { nullptr };
        ValueType element;
        std::atomic<bool> deleted { false };
        id node_id;
    };

private:
    std::atomic<node *> head_ { nullptr };
    mutable std::atomic<std::size_t> block_gc_ { 0 };
    std::atomic<bool> gc_active_ { false };
    std::atomic<id> last_id_ { 0 };
};

} }
