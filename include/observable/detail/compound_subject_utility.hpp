#pragma once
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>

namespace observable { namespace detail { namespace compound_subject {

//! Call a function one or more times with the Ith element of a tuple.
template <std::size_t I, typename ... T>
struct apply_to;

//! Call a function once.
template <std::size_t I>
struct apply_to<I>
{
    template <typename Fun, typename Tuple>
    static constexpr auto apply(Fun && fun, Tuple && tuple)
    {
        return fun(std::get<I>(tuple));
    }
};

//! Recursive version. Call a function multiple times.
template <std::size_t I, typename Next>
struct apply_to<I, Next> : Next
{
    template <typename Fun, typename Tuple>
    static constexpr auto apply(Fun && fun, Tuple && tuple)
    {
        fun(std::get<I>(tuple));
        Next::apply(fun, tuple);
    }
};

//! Stop compilation with an error.
template <typename T>
struct fail
{
    static_assert(std::is_same<T, void>::value,
                  "The provided callable is not compatible with any of the "
                  "compound subject's declared function types.");
};

//! Choose the TrueType if the CallableType can be stored in a
//! std::function<FunctionType> and the FalseType otherwise.
template <typename CallableType,
          typename FunctionType,
          typename TrueType,
          typename FalseType>
using choose_next = typename std::conditional<
                                std::is_convertible<
                                    CallableType,
                                    std::function<FunctionType>
                                >::value,
                                TrueType,
                                FalseType
                            >::type;

//! Provide an apply() method that will call a function once, with the first
//! subject that CallableType is compatible with.
template <typename CallableType, std::size_t I, typename ... T>
struct apply_first_type;

template <typename CallableType,
          std::size_t I,
          typename Head, typename ... Tail>
struct apply_first_type<CallableType, I, Head, Tail...> :
    choose_next<CallableType,
                typename Head::observer_type,
                apply_to<I>,
                apply_first_type<CallableType, I + 1, Tail...>>
{
};

template <typename CallableType, std::size_t I>
struct apply_first_type<CallableType, I> : fail<CallableType>
{
};

//! Provide an apply() method that will call a function for all subjects that
//! CallableType is compatible with.
template <typename CallableType,
          std::size_t I,
          bool FoundSubject,
          typename ... T>
struct apply_all_type;

template <typename CallableType,
          std::size_t I,
          bool FoundSubject,
          typename Head, typename ... Tail>
struct apply_all_type<CallableType, I, FoundSubject, Head, Tail...> :
    choose_next<CallableType,
                typename Head::observer_type,
                apply_to<I, apply_all_type<CallableType, I + 1, true, Tail...>>,
                apply_all_type<CallableType, I + 1, FoundSubject, Tail...>>
{
};

template <typename CallableType, std::size_t I>
struct apply_all_type<CallableType, I, false> : fail<CallableType>
{
};

template <typename CallableType, std::size_t I>
struct apply_all_type<CallableType, I, true>
{
    template <typename ... T>
    static constexpr auto apply(T && ...) { }
};

//! Wrapper around apply_first_type. Will call ``fun`` with the first compatible
//! subject and return the result.
template <typename CallableType, typename Fun, typename ... Subjects>
constexpr auto apply_first(Fun && fun, std::tuple<Subjects ...> & subjects)
{
    return apply_first_type<CallableType, 0, Subjects ...>::apply(fun, subjects);
}

//! Wrapper around apply_all_type. Will call ``fun`` with all compatible
//! subjects.
template <typename CallableType, typename Fun, typename ... Subjects>
constexpr auto apply_all(Fun && fun, std::tuple<Subjects ...> const & subjects)
{
    apply_all_type<CallableType, 0, false, Subjects ...>::apply(fun, subjects);
}

} } }
