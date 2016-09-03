#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace observable { namespace detail {

//! Collection that can hold and call heterogeneous, generic functions.
//!
//! \note This class needs RTTI to be enabled.
//! \warning Methods in this class cannot be safely called concurrently.
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
        auto const ptr = std::make_shared<std::function<Signature>>(std::move(function));

        functions_.emplace(key<Signature>(), ptr);
        return make_id(ptr);
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
        auto const it = find_if(begin(functions_),
                                end(functions_),
                                [&](auto const & kv) {
                                    return this->make_id(kv.second) == id;
                                });

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
    //! \warning Do not change the function collection while a call is in
    //!          progress.
    //! \return The number of functions that were called.
    template <typename Signature, typename ... Arguments>
    auto call_all(Arguments && ... arguments) const
    {
        std::size_t call_count = 0;
        auto const range = functions_.equal_range(key<Signature>());
        for(auto i = range.first; i != range.second; ++i, ++call_count)
        {
            auto const f = reinterpret_cast<std::function<Signature> *>(i->second.get());
            assert(f);
            (void)((*f)(std::forward<Arguments>(arguments) ...));
        }

        return call_count;
    }

    //! Check if a function id belongs to this collection.
    auto contains(function_id const & id) const
    {
        return find_if(begin(functions_),
                       end(functions_),
                       [&](auto const & kv) {
                            return this->make_id(kv.second) == id;
                        })
               != end(functions_);
    }

    //! Retrieve the number of functions in the collection.
    auto size() const noexcept
    {
        return functions_.size();
    }

    //! Returns true if the collection is empty.
    auto empty() const noexcept
    {
        return functions_.empty();
    }

private:
    using function_key = std::type_index;

    //! Retrieve the type index of the provided type.
    template <typename FunctionSignature>
    static auto key()
    {
        return function_key { typeid(FunctionSignature) };
    }

    //! Retrieve a unique id for a function pointer.
    static auto make_id(std::shared_ptr<void> const & function) noexcept
    {
        return reinterpret_cast<std::size_t>(function.get());
    }

private:
    std::unordered_multimap<function_key, std::shared_ptr<void>> functions_;
};

} }
