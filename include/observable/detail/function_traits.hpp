#pragma once
#include <cstddef>
#include <type_traits>

namespace observable { namespace detail {

//! Function type traits.
//!
//! The following member types are provided:
//!
//! - type: Function signature.
//!
//! - return_type: The function's return type.
//!
//! The following static value is provided:
//!
//! - arity: The number of arguments.
template <typename Functor>
struct function_traits;

namespace helper {
    template <typename Functor>
    struct traits;

    // Unpack functions
    template <typename Return, typename ... Arguments>
    struct traits<Return(Arguments ...)>
    {
        using type = Return(Arguments ...);

        using return_type = Return;

        static constexpr std::size_t arity = sizeof...(Arguments);
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
    struct traits<Return(Functor::*)(Arguments...) const> :
        traits<Return(Arguments...)>
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
                                typename std::remove_pointer<Functor>::type
                            >::type>
{
};

//! Add a new argument to a function's type.
//!
//! The ``type`` member type will hold the new function type.
template <typename NewArgument, typename Functor>
struct prepend_argument : prepend_argument<
                                    NewArgument,
                                    typename function_traits<Functor>::type>
{
};

template <typename NewArgument, typename Return, typename ... Arguments>
struct prepend_argument<NewArgument, Return(Arguments ...)>
{
    using type = Return(NewArgument, Arguments ...);
};

//! Convert the function's type to one where all arguments are const references.
template <typename Functor>
struct const_ref_args : const_ref_args<typename function_traits<Functor>::type>
{
};

template <typename Return, typename ... Arguments>
struct const_ref_args<Return(Arguments ...)>
{
    using type = Return(typename std::add_lvalue_reference<
                            typename std::add_const<
                                typename std::remove_reference<Arguments>::type
                            >::type
                        >::type ...);
};

} }
