#pragma once
#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include "function_wrapper.hpp"

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
    using function_id = function_wrapper::id_type;

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
        functions_.emplace_back(std::move(function),
                                type_index<FunctionType>(),
                                ++last_id);

        return functions_.back().instance_id();
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
                                [&](auto && w) { return w.instance_id() == id; });

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
    //!       This method will return once all call functions have returned.
    //! \warning Return values of the called functions will be ignored.
    //! \warning Do not change the function collection while a call is in
    //!          progress.
    //! \return The number of functions that were called.
    template <typename FunctionType, typename ... Arguments>
    auto call_all(Arguments && ... arguments) const
    {
        auto const type_id = type_index<FunctionType>();

        std::size_t call_count = 0;
        for(auto && wrapper : functions_)
        {
            if(wrapper.type_id() != type_id)
                continue;

            wrapper.function<FunctionType>()(arguments ...);
            ++call_count;
        }

        return call_count;
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
    //! Retrieve an identifier for the specified function type.
    template <typename FunctionType>
    std::size_t type_index() const
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
    std::size_t last_id = 0;
};

} }
