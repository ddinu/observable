#pragma once

namespace observable { namespace detail { namespace subject {

struct unspecialized_copy_policy_;

//! Not copyable but movable.
template <typename T=unspecialized_copy_policy_>
struct no_copy
{
    template <typename U>
    using rebind = no_copy<U>;

    no_copy() =default;
    no_copy(no_copy const &) =delete;
    no_copy(no_copy &&) =default;
    no_copy & operator=(no_copy const &) =delete;
    no_copy & operator=(no_copy &&) =default;
};

//! Shallow copy, the class will be copied but all data will be shared.
template <typename T=unspecialized_copy_policy_>
struct shallow_copy
{
    template <typename U>
    using rebind = shallow_copy<U>;
};

//! The class will be fully copied.
template <typename T=unspecialized_copy_policy_>
struct deep_copy
{
    template <typename U>
    using rebind = deep_copy<U>;
};

} } }
