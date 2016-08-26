#pragma once
#include <unordered_set>
#include "function_collection.hpp"

namespace observable { namespace detail {

//! Store a relation between a function id and one or more function collection
//! and allow easy removal of the function from the collections.
class function_handle
{
public:
    //! Create a new handle for the provided function id.
    explicit function_handle(function_collection::function_id const & id) :
        id_(id)
    {
    }

    //! Remove the associated function from all registered collections.
    //!
    //! \note This method will only work once. Any additional calls will do
    //!       nothing.
    void remove()
    {
        if(removed_)
            return;

        for(auto && c : collections_)
            c->remove(id_);

        collections_.clear();
        removed_ = true;
    }

    //! Register a collection that holds the associated function.
    //!
    //! The associated function will be removed from all registered collections.
    void register_collection(function_collection * collection)
    {
        collections_.emplace(collection);
    }

    //! Unregister the provided collection.
    void unregister_collection(function_collection * collection)
    {
        auto it = collections_.find(collection);
        if(it == end(collections_))
            return;

        collections_.erase(it);
    }

    function_collection::function_id function_id() const { return id_; }

    //! A function handle is not copy constructible.
    function_handle(function_handle const &) =delete;

    //! A function handle is move constructible.
    function_handle(function_handle &&) =default;

    //! A function handle is not copy assignable.
    function_handle & operator=(function_handle const &) =delete;

    //! A function handle is move assignable.
    function_handle & operator=(function_handle &&) =default;

private:
    function_collection::function_id id_;
    std::unordered_set<function_collection *> collections_;
    bool removed_ = false;
};

} }
