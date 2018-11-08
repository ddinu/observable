#pragma once
#include <atomic>
#include <type_traits>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable { namespace detail {

//! Thread-safe collection that can store arbitrary items and apply a functor over
//! them.
//!
//! All methods of the collection can be safely called in parallel, from multiple
//! threads.
//!
//! \warning The order of elements inside the collection is unspecified.
//!
//! \tparam ValueType Type of the elements that will be stored inside the
//!                   collection. This type must be at least move constructible.
//! \ingroup observable_detail
template <typename ValueType>
class collection final
{
public:
    //! Identifier for an element that has been inserted. You can use this id to
    //! remove a previously inserted element.
    using id = std::size_t;

    //! Create an empty collection.
    collection() noexcept = default;

    //! Insert a new element into the collection.
    //!
    //! \param[in] element The object to be inserted.
    //! \tparam ValueType_ Type of the inserted element. Must be convertible to
    //!                    the collection's ValueType.
    //!
    //! \return An \ref id that can be used to remove the inserted element.
    //!         This \ref id is stable; you can use it after modifying the
    //!         collection.
    //!
    //! \note Any apply() call running concurrently with an insert() that has
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
            auto const block_gc = gc_blocker { this };

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
    //! If no element with the provided \ref id exists, this method does nothing.
    //!
    //! \param[in] element_id Id of the element to remove. This is returned by
    //!                       insert.
    //!
    //! \return True if an element of the collection was removed, false if no
    //!         element has been removed.
    //!
    //! \note Any apply() call running concurrently with a remove() call that has
    //!       not already called its functor with the removed element, is
    //!       guaranteed to not call the functor with the removed element.
    auto remove(id const & element_id) noexcept
    {
        auto deleted = false;
        {
            auto const block_gc = gc_blocker { this };

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

    //! Apply a unary functor over all elements of the collection.
    //!
    //! The functor will be called multiple times, with each element, in an
    //! unspecified order.
    //!
    //! \note This method is reentrant; you can call insert() and remove() on the
    //!       collection from inside the functor.
    //!
    //! \note It is well defined and supported to remove() the element passed
    //!       to the functor, even before the functor returns.
    //!
    //! \param[in] fun A functor that will be called with each element of the
    //!                collection. The functor must be assignable to a
    //!                ``std::function<void(ValueType const &)>``.
    //!
    //! \tparam UnaryFunctor Type of the ``fun`` parameter.
    template <typename UnaryFunctor>
    void apply(UnaryFunctor && fun) const
        noexcept(noexcept(fun(std::declval<ValueType>())))
    {
        auto const block_gc = gc_blocker { this };

        for(auto n = head_.load(); n; n = n->next)
        {
            if(n->deleted.load())
                continue;

            fun(n->element);
        }
    }

    //! Return true if the collection has no elements.
    auto empty() const noexcept
    {
        auto const block_gc = gc_blocker { this };
        auto const h = head_.load();
        return !h || h->deleted;
    }

    //! Destructor.
    ~collection() noexcept
    {
        for(auto n = head_.load(); n; n = n->next)
            n->deleted.store(true);

        gc();
    }

public:
    //! Collections are not copy-constructible.
    collection(collection const &) =delete;

    //! Collections are not copy-assignable.
    auto operator=(collection const &) -> collection & =delete;

    //! Collections are move-constructible.
    collection(collection &&) =delete;

    //! Collections are move-assignable.
    auto operator=(collection &&) -> collection & =delete;

private:
    //! Delete any nodes marked as deleted.
    void gc() noexcept
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
        explicit gc_blocker(collection<ValueType> const * c) noexcept :
            collection_{ c }
        {
            ++collection_->block_gc_;
            while(collection_->gc_active_.load())
                ;
        }

        ~gc_blocker() noexcept
        {
            --collection_->block_gc_;
        }

        gc_blocker() =delete;
        gc_blocker(gc_blocker const &) =delete;
        auto operator=(gc_blocker const &) -> gc_blocker & =delete;
        gc_blocker(gc_blocker &&) =default;
        auto operator=(gc_blocker &&) -> gc_blocker & =default;

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

OBSERVABLE_END_CONFIGURE_WARNINGS
