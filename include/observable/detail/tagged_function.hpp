#pragma once
#include <type_traits>
#include "function_traits.hpp"

namespace observable { namespace detail {

//! Dummy tag value that signifies that the function is not tagged.
struct no_tag { };
inline bool operator==(no_tag const &, no_tag const &) { return true; }
inline bool operator!=(no_tag const &, no_tag const &) { return false; }

//! Function tag wrapper.
template <typename T>
struct tag
{
    //! Create a new tag.
    explicit tag(T v) noexcept(std::is_nothrow_move_constructible<T>::value) :
        value(v)
    {
    }

    T value;
};

template <typename T>
inline bool operator==(tag<T> const & a, tag<T> const & b)
{
    return a.value == b.value;
}

template <typename T>
inline bool operator!=(tag<T> const & a, tag<T> const & b)
{
    return a.value != b.value;
}

//! Tagged function.
template <typename Tag, typename Function>
struct tagged
{
    //! Tagged function signature.
    using type = typename observable::detail::prepend_argument<
                            Tag,
                            typename observable::detail::function_traits<Function>::normalized
                 >::type;

    //! Create a new tagged function.
    tagged(Tag tag, Function function) noexcept(std::is_nothrow_move_constructible<Tag>::value &&
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

} }
