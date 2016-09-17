#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

namespace observable { namespace detail {

//! Collection that can hold and call types that satisfy the Callable concept.
//!
//! \tparam FunctionType Function type compatible with the callables that will
//!                      be stored.
//! \warning Methods in this class cannot be safely called concurrently.
template <typename FunctionType>
class callable_collection final
{
public:
    //! Identifier for a callable that has been inserted. You can use this to
    //! remove a previously inserted object.
    using id = std::size_t;

    //! Insert a callable object into the collection.
    //!
    //! \param callable The object to be inserted.
    //! \return Id that can be used to remove the callable from the collection.
    template <typename Callable>
    auto insert(Callable && callable)
    {
        functions_.emplace_back(functions_.empty() ? 1 : functions_.back().first + 1,
                                std::forward<Callable>(callable));

        return functions_.back().first;
    }

    //! Remove a previously inserted callable object from the collection.
    //!
    //! If no object with the provided id exists in the collection, this
    //! method does nothing.
    //!
    //! \param id Id returned by insert.
    //! \return True if the a member of the collection was removed.
    auto remove(id const & id)
    {
        auto const it = find_if(begin(functions_),
                                end(functions_),
                                [&](auto && v) {
                                    return v.first == id;
                                });

        if(it == end(functions_))
            return false;

        functions_.erase(it);
        return true;
    }

    //! Call all stored callable objects with the provided arguments.
    //!
    //! \param arguments The arguments to pass to all objects that will be called.
    //! \tparam Arguments Types of the arguments.
    //! \note All calls will be made on the thread that calls this method.
    //!       This method will return once all calls have returned.
    //! \warning Return values will be ignored.
    //! \warning Do not change the collection while a call is in progress.
    //! \warning The calls will be made in an unspecified order.
    template <typename ... Arguments>
    auto call_all(Arguments && ... arguments) const
    {
        for(auto && fun : functions_)
            fun.second(arguments ...);
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
    std::vector<std::pair<std::size_t, std::function<FunctionType>>> functions_;
};

} }
