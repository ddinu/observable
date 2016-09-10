#pragma once
#include <type_traits>
#include "function_traits.hpp"

namespace observable { namespace detail {

//! Tagged function.
template <typename Tag, typename Function>
struct tagged
{
    //! New type of the tagged function.
    using type = typename prepend_argument<
                    Tag,
                    typename function_traits<Function>::type
                 >::type;

    //! Create a new tagged function.
    tagged(Tag tag, Function function)
        noexcept(std::is_nothrow_move_constructible<Tag>::value &&
                 std::is_nothrow_move_constructible<Function>::value) :
        tag_(std::move(tag)),
        function_(std::move(function))
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
