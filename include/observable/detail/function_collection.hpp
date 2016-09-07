#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
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
    //! \tparam FunctionType The function's type.
    //! \return Function id that can be used to remove the function from the
    //!         collection.
    template <typename FunctionType>
    auto insert(std::function<FunctionType> function)
    {
        assert(function);
        functions_.emplace_back(std::move(function), type_index<FunctionType>());

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
                                [&](auto && fun) { return fun.id() == id; });

        if(it == end(functions_))
            return false;

        functions_.erase(it);
        return true;
    }

    //! Call all functions that match the specified function signature with the
    //! provided arguments.
    //!
    //! If no function with the provided signature exists, this method does
    //! nothing.
    //!
    //! \param arguments The arguments to pass to all functions that will be
    //!                  called.
    //! \tparam FunctionType The type of the functions that will be called.
    //! \tparam Arguments Types of the function's arguments.
    //! \note All functions will be called on the thread that calls this method.
    //!       This method will return once all functions have been called.
    //! \warning Return values of the called functions will be ignored.
    //! \warning Do not change the function collection while a call is in
    //!          progress.
    //! \return The number of functions that were called.
    template <typename FunctionType, typename ... Arguments>
    auto call_all(Arguments && ... arguments) const
    {
        auto const ti = type_index<FunctionType>();

        std::size_t call_count = 0;
        for(auto && fun : functions_)
        {
            if(fun.type != ti)
                continue;

            fun.call<FunctionType>(arguments ...);
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

    //! Check if the collection is empty.
    auto empty() const noexcept
    {
        return functions_.empty();
    }

private:
    //! Wrap a function and hide its type.
    struct function_wrapper
    {
        std::shared_ptr<void> function;
        std::size_t type;

        //! Retrieve the wrapped function's id.
        auto id() const noexcept ->std::size_t
        {
            return reinterpret_cast<std::size_t>(function.get());
        }

        //! Call the wrapped function with the provided arguments.
        template <typename FunctionType, typename ... Arguments>
        auto call(Arguments && ... arguments) const -> void
        {
            auto const f = reinterpret_cast<std::function<FunctionType> *>(
                                                            function.get());
            assert(f);
            (void)((*f)(arguments ...));
        }

        //! Create a new function wrapper.
        template <typename FunctionType>
        function_wrapper(std::function<FunctionType> function, std::size_t ti) :
            function(std::make_shared<std::function<FunctionType>>(std::move(function))),
            type(ti)
        {
        }
    };

    //! Retrieve an identifier for the specified function type.
    template <typename FunctionType>
    auto type_index() const -> std::size_t
    {
        static auto type = &typeid(FunctionType);

        auto const size = types_.size();
        for(std::size_t i = 0; i < size; ++i)
            if(types_[i] == type)
                return i;

        for(std::size_t i = 0; i < size; ++i)
            if(*types_[i] == *type)
                return i;

        types_.emplace_back(type);
        return types_.size() - 1;
    }

private:
    std::vector<function_wrapper> functions_;
    mutable std::vector<std::type_info const *> types_;
};

} }
