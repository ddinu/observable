#include <functional>
#include <type_traits>
#include <gtest/gtest.h>
#include "observable/detail/function_traits.hpp"

namespace observable { namespace detail { namespace test {

// Fixture.
template <typename FunctionTypes>
struct function_traits_test : testing::Test, FunctionTypes { };

// Used to make the function types conveniently available to the fixture.
template <typename Arity_0_Type, typename Arity_3_Type, bool IsFunctor>
struct test_functions
{
    using arity_0_type = Arity_0_Type;
    using arity_3_type = Arity_3_Type;
    static constexpr bool const is_functor = IsFunctor;
};

namespace funs {
    // Various functions used for testing.

    namespace {
        void arity_0_f() { }
        double arity_3_f(int &, float const, char const &) { return 0; }

        auto arity_0_lambda_v = [=]() -> void { };
        auto arity_3_lambda_v = [=](int &, float const, char const &) -> double { return 0; };

        auto arity_0_mut_lambda_v = [=]() mutable -> void { };
        auto arity_3_mut_lambda_v = [=](int &, float const, char const &) mutable -> double { return 0; };
    }

    using arity_0 = decltype(arity_0_f);
    using arity_3 = decltype(arity_3_f);
    using arity_0_lambda = decltype(arity_0_lambda_v);
    using arity_3_lambda = decltype(arity_3_lambda_v);
    using arity_0_mut_lambda = decltype(arity_0_mut_lambda_v);
    using arity_3_mut_lambda = decltype(arity_3_mut_lambda_v);
    using arity_0_generic = std::function<void()>;
    using arity_3_generic = std::function<double(int &, float const, char const &)>;
}

// Test configuration.
using functions = testing::Types<
    // Function
    test_functions<funs::arity_0, funs::arity_3, false>,
    test_functions<funs::arity_0 &, funs::arity_3 &, false>,
    test_functions<funs::arity_0 &&, funs::arity_3 &&, false>,
    test_functions<funs::arity_0 *, funs::arity_3 *, false>,
    test_functions<funs::arity_0 * const, funs::arity_3 * const, false>,

    // Lambda
    test_functions<funs::arity_0_lambda, funs::arity_3_lambda, true>,
    test_functions<funs::arity_0_lambda &, funs::arity_3_lambda &, true>,
    test_functions<funs::arity_0_lambda const &, funs::arity_3_lambda const &, true>,
    test_functions<funs::arity_0_lambda &&, funs::arity_3_lambda &&, true>,
    test_functions<funs::arity_0_lambda const &&, funs::arity_3_lambda const &&, true>,
    test_functions<funs::arity_0_lambda *, funs::arity_3_lambda *, true>,
    test_functions<funs::arity_0_lambda const *, funs::arity_3_lambda const *, true>,
    test_functions<funs::arity_0_lambda const * const, funs::arity_3_lambda const * const, true>,

    // Mutable lambda
    test_functions<funs::arity_0_mut_lambda, funs::arity_3_mut_lambda, true>,

    // std::function<>
    test_functions<funs::arity_0_generic, funs::arity_3_generic, true>
>;

TYPED_TEST_CASE(function_traits_test, functions);

TYPED_TEST(function_traits_test, can_get_return_type)
{
    using return_type = function_traits<arity_3_type>::return_type;
    ASSERT_TRUE((std::is_same_v<return_type, double>));
}

TYPED_TEST(function_traits_test, can_get_void_return_type)
{
    using return_type = function_traits<arity_0_type>::return_type;
    ASSERT_TRUE((std::is_same_v<return_type, void>));
}

TYPED_TEST(function_traits_test, can_get_arity)
{
    auto arity = function_traits<arity_3_type>::arity;
    ASSERT_EQ(arity, 3);
}

TYPED_TEST(function_traits_test, arity_is_zero_for_no_args)
{
    auto arity = function_traits<arity_0_type>::arity;
    ASSERT_EQ(arity, 0);
}

TYPED_TEST(function_traits_test, is_functor_is_correctly_set)
{
    ASSERT_EQ(function_traits<arity_0_type>::is_functor, is_functor);
}

TYPED_TEST(function_traits_test, normalized_signature_is_correct)
{
    using normalized = function_traits<arity_3_type>::normalized;
    ASSERT_TRUE((std::is_same_v<normalized, double(int, float, char)>));
}

TYPED_TEST(function_traits_test, signature_is_correct)
{
    using signature = function_traits<arity_3_type>::signature;
    ASSERT_TRUE((std::is_same_v<signature, double(int &, float const, char const &)>));
}

} } }
