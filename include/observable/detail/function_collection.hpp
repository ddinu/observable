#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace observable { namespace detail {

//! Collection that can hold and call heterogeneous, generic functions.
//!
//! \note This class needs RTTI to be enabled.
//! \warning All methods can be safely called in parallel.
class function_collection
{
public:
    //! Identifier for a function that has been inserted. You can use this to
    //! remove a previously inserted function.
    using function_id = std::size_t;

    //! Insert a function into the collection.
    //!
    //! \param function The function to be inserted.
    //! \tparam Signature The function's type (i.e. not a function pointer, but
    //!                   the actual function type, for example: ``int(double)``).
    //! \return Function id that can be used to remove the function from the
    //!         collection.
    template <typename Signature>
    auto insert(std::function<Signature> function)
    {
        assert(function);
        function_wrapper w { std::move(function) };
        auto id = w.id;

        std::lock_guard<std::mutex> lock { mutex_ };
        functions_.emplace(key<Signature>(), std::move(w));
        return id;
    }

    //! Remove a function from the collection.
    //!
    //! If no function with the provided id exists in the collection, this method
    //! does nothing.
    //!
    //! \param id Function id returned by insert.
    //! \return True if the function was a member of the collection and has been
    //!         removed.
    auto remove(function_id const & id)
    {
        std::lock_guard<std::mutex> lock { mutex_ };

        auto it = find_if(begin(functions_),
                          end(functions_),
                          [&](auto && kv) { return kv.second.id == id; });

        if(it == end(functions_))
            return false;

        functions_.erase(it);
        return true;
    }

    //! Call all functions that match the provided function signature with the
    //! provided arguments.
    //!
    //! If no function with the provided signature exists, this method does
    //! nothing.
    //!
    //! \param arguments The arguments to pass to all functions that will be
    //!                  called.
    //! \tparam Signature The signature of the functions that will be called.
    //! \tparam Arguments Types of the function's arguments.
    //! \note All functions will be called on the thread that calls this method.
    //!       This method will return once all functions have been called.
    //! \warning Return values of the called functions will be ignored.
    //! \return The number of functions that were called.
    template <typename Signature, typename ... Arguments>
    auto call_all(Arguments && ... arguments) const
    {
        std::vector<function_wrapper> snapshot;

        {
            std::lock_guard<std::mutex> lock { mutex_ };

            auto range = functions_.equal_range(key<Signature>());
            snapshot.resize(static_cast<std::size_t>(
                                std::distance(range.first, range.second)));

            std::transform(range.first,
                           range.second,
                           begin(snapshot),
                           [](auto && kv) { return kv.second; });
        }

        for(auto && w : snapshot)
            w.call<Signature>(arguments ...);

        return snapshot.size();
    }

    //! Check if a function id belongs to this collection.
    auto contains(function_id const & id) const
    {
        std::lock_guard<std::mutex> lock { mutex_ };

        return find_if(begin(functions_),
                       end(functions_),
                       [&](auto && kv) { return kv.second.id == id; })
               != end(functions_);
    }

    //! Retrieve the number of functions in the collection.
    auto size() const
    {
        std::lock_guard<std::mutex> lock { mutex_ };
        return functions_.size();
    }

    //! Returns true if the collection is empty.
    auto empty() const
    {
        std::lock_guard<std::mutex> lock { mutex_ };
        return functions_.empty();
    }

public:
    //! Create an empty collection.
    function_collection() =default;

    //! Collections are copy constructible.
    function_collection(function_collection const & other)
    {
        std::lock_guard<std::mutex> lock_other { other.mutex_ };
        functions_ = other.functions_;
    }

    //! Collections are move constructible.
    function_collection(function_collection && other) :
        functions_(std::move(other.functions_))
    {
    }

    //! Collections are copy and move assignable.
    function_collection & operator=(function_collection other)
    {
        swap(*this, other);
        return *this;
    }

    //! Swap two collections.
    friend void swap(function_collection & a, function_collection & b)
    {
        std::lock_guard<std::mutex> lock_a { a.mutex_ };
        std::lock_guard<std::mutex> lock_b { b.mutex_ };

        using std::swap;
        swap(a.functions_, b.functions_);
    }

private:
    using function_key = std::type_index;

    //! Retrieve the type index of the provided type.
    template <typename FunctionSignature>
    static auto key() -> function_key
    {
        return { typeid(FunctionSignature) };
    }

private:
    //! Helper to manage stored functions.
    struct function_wrapper
    {
        std::size_t id;
        std::shared_ptr<void> function_ptr;

        //! Create an invalid wrapper.
        function_wrapper() =default;

        //! Create a function wrapper.
        template <typename Signature>
        function_wrapper(std::function<Signature> function) :
            function_ptr(new std::function<Signature> { std::move(function) })
        {
            id = reinterpret_cast<std::size_t>(function_ptr.get());
        }

        //! Call the wrapped function.
        template <typename Signature, typename ... Arguments>
        void call(Arguments && ... arguments)
        {
            auto f = reinterpret_cast<std::function<Signature> *>(function_ptr.get());
            assert(*f);

            (void)(*f)(arguments ...);
        }
    };

    std::unordered_multimap<function_key,
                            function_wrapper> functions_; //!< Functions by type.

    mutable std::mutex mutex_;
};

} }
