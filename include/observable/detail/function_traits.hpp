#pragma once
#include <cstddef>
#include <type_traits>

namespace observable { namespace detail {

//! Function type traits.
//!
//! The following member types are provided:
//!
//! - signature: Function signature.
//!
//! - normalized: Normalized function signature with const and references removed
//!               from arguments.
//!
//! - return_type: The function's return type.
//!
//! The following static value is provided:
//!
//! - arity: The number of arguments.
template <typename Functor>
struct function_traits;

namespace helper {
    // Normalize an argument type.
    template <typename T> struct norm { using type = T; };
    template <typename T> struct norm<T const> { using type = T; };
    template <typename T> struct norm<T const &> { using type = T; };

    template <typename Functor>
    struct traits;

    // Unpack functions
    template <typename Return, typename ... Arguments>
    struct traits<Return(Arguments ...)>
    {
        using signature = Return(Arguments ...);

        using normalized = Return(typename norm<Arguments>::type ...);

        using return_type = Return;

        static constexpr std::size_t const arity = sizeof...(Arguments);
    };

    // Unpack function pointers.
    template <typename Return, typename ... Arguments>
    struct traits<Return(*)(Arguments...)> : traits<Return(Arguments ...)>
    {
    };

    // Unpack class methods.
    template <typename Return, typename Functor, typename ... Arguments>
    struct traits<Return(Functor::*)(Arguments...)> : traits<Return(Arguments...)>
    {
    };

    // Unpack const class methods.
    template <typename Return, typename Functor, typename ... Arguments>
    struct traits<Return(Functor::*)(Arguments...) const> : traits<Return(Arguments...)>
    {
    };

    // Unpack the call operator (for functors).
    template <typename Functor>
    struct traits : traits<decltype(&Functor::operator())>
    {
    };
}

template <typename Functor>
struct function_traits : helper::traits<
                            typename std::decay<
                                typename std::remove_pointer<Functor>::type>::type>
{
};

} }
