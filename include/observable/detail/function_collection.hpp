#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <vector>
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

        functions_.emplace_back(
            std::make_shared<std::function<Signature>>(std::move(function)),
            &typeid(Signature)
        );

        return functions_.back().id();
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
                                [&](auto const & fun) {
                                    return fun.id() == id;
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
        static auto const * type = &typeid(Signature);

        std::size_t call_count = 0;
        for(auto && fun : functions_)
        {
            if(*fun.type != *type)
                continue;

            fun.call<Signature>(arguments ...);
            ++call_count;
        }

        return call_count;
    }

    //! Check if a function id belongs to this collection.
    auto contains(function_id const & id) const
    {
        return find_if(begin(functions_),
                       end(functions_),
                       [&](auto const & fun) {
                            return fun.id() == id;
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
    struct function_wrapper
    {
        std::shared_ptr<void> function_ptr;
        std::type_info const * type;

        auto id() const noexcept
        {
            return reinterpret_cast<std::size_t>(function_ptr.get());
        }

        template <typename Signature, typename ... Arguments>
        void call(Arguments && ... arguments) const
        {
            auto const f = reinterpret_cast<std::function<Signature> *>(function_ptr.get());
            assert(f);
            (void)((*f)(arguments ...));
        }

        function_wrapper(std::shared_ptr<void> const & function_ptr,
                         std::type_info const * type) :
            function_ptr(std::move(function_ptr)),
            type(type)
        {
        }
    };

private:
    std::vector<function_wrapper> functions_;
};

} }
