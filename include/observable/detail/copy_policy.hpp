#pragma once
#include <cassert>
#include <memory>
#include <unordered_map>
#include "function_collection.hpp"

namespace observable { namespace detail {

//! Not copyable but movable.
struct no_copy
{
    no_copy() =default;
    no_copy(no_copy const &) =delete;
    no_copy & operator=(no_copy const &) =delete;
    no_copy(no_copy &&) =default;
    no_copy & operator=(no_copy &&) =default;

protected:
    template <typename T>
    static auto copy_impl(T &, T const &)
    {
        assert(!"Not implemented");
    }
};

//! Shallow copy, the class will be copied but all data will be shared.
struct shallow_copy
{
protected:
    static auto copy_impl(std::shared_ptr<function_collection> & target,
                          std::shared_ptr<function_collection> const & source)
    {
        target = source;
    }

    template <typename Tag>
    static auto copy_impl(std::shared_ptr<std::unordered_map<Tag, function_collection>> & target,
                          std::shared_ptr<std::unordered_map<Tag, function_collection>> const & source)
    {
        target = source;
    }
};

} }
