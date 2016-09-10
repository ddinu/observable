#pragma once
#include <type_traits>
#include "function_traits.hpp"

namespace observable { namespace detail {

//! Tagged function. This function wraps a regular function and only calls it if
//! the tag specified when calling it matches the tag specified when creating the
//! function.
//!
//! \tparam Tag The tag type. This type must satisfy the EqualityComparable
//!             concept.
//! \tparam Function Type of the wrapped function. This type must satisfy the
//!                  Callable concept.
template <typename Tag, typename Function>
struct tagged
{
    //! New type of the tagged function.
    using type = typename prepend_argument<
                    Tag,
                    typename function_traits<Function>::type
                 >::type;

    //! Create a new tagged function.
    template <typename T, typename F>
    tagged(T && tag, F && function)
        noexcept(
            noexcept(Tag { std::forward<T>(tag) }) &&
            noexcept(Function( std::forward<F>(function) ))
        ):
        tag_ { std::forward<T>(tag) },
        function_( std::forward<F>(function) ) // { function }, triggers MSVC bug.
    {
    }

    //! Call the stored function if the provided tag matches the stored tag.
    template <typename ... Arguments>
    void operator()(Tag const & tag, Arguments && ... args)
    {
        if(tag != tag_)
            return;

        function_(std::forward<Arguments>(args) ...);
    }

private:
    Tag tag_;
    Function function_;
};

//! Dummy tag value useful for untagged functions.
struct no_tag { constexpr no_tag() noexcept =default; };
inline bool operator==(no_tag const &, no_tag const &) { return true; }
inline bool operator!=(no_tag const &, no_tag const &) { return false; }

} }
