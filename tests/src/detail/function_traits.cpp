#include <functional>
#include <type_traits>
#include "gtest.h"
#include "observable/detail/function_traits.hpp"

namespace observable { namespace detail { namespace test {

TEST(function_traits_test, can_get_function_return_type)
{
    using type = typename function_traits<double()>::return_type;
    ASSERT_TRUE((std::is_same<type, double>::value));
}

TEST(function_traits_test, can_get_function_pointer_return_type)
{
    using type = typename function_traits<double(*)()>::return_type;
    ASSERT_TRUE((std::is_same<type, double>::value));
}

TEST(function_traits_test, can_get_lambda_return_type)
{
    auto lambda = [=]() -> double { return 5.0; };
    using type = typename function_traits<decltype(lambda)>::return_type;
    ASSERT_TRUE((std::is_same<type, double>::value));
}

TEST(function_traits_test, can_get_function_arity)
{
    auto arity = function_traits<void(int, double, char)>::arity;
    ASSERT_EQ(arity, 3u);
}

TEST(function_traits_test, can_get_function_pointer_arity)
{
    auto arity = function_traits<void(*)(int, double, char)>::arity;
    ASSERT_EQ(arity, 3u);
}

TEST(function_traits_test, can_get_lambda_arity)
{
    auto lambda = [=](int, double, char) { };
    auto arity = function_traits<decltype(lambda)>::arity;
    ASSERT_EQ(arity, 3u);
}

TEST(function_traits_test, arity_is_zero_for_no_args)
{
    auto arity = function_traits<void()>::arity;
    ASSERT_EQ(arity, 0u);
}

TEST(function_traits_test, type_removes_function_pointer)
{
    using type = typename function_traits<void(*)()>::type;
    ASSERT_TRUE((std::is_same<type, void()>::value));
}

TEST(function_traits_test, type_is_extracted_from_lambda)
{
    auto lambda = [=](int, double) -> char { return 'a'; };
    using type = typename function_traits<decltype(lambda)>::type;
    ASSERT_TRUE((std::is_same<type, char(int, double)>::value));
}

TEST(function_traits_test, type_preserves_const_arguments)
{
    using function = void(int const, char const);
    using type = typename function_traits<function>::type;
    ASSERT_TRUE((std::is_same<type, function>::value));
}

TEST(function_traits_test, type_preserves_ref_arguments)
{
    using function = void(int &, char &);
    using type = typename function_traits<function>::type;
    ASSERT_TRUE((std::is_same<type, function>::value));
}

TEST(function_traits_test, type_preserves_const_ref_arguments)
{
    using function = void(int const &, char const &);
    using type = typename function_traits<function>::type;
    ASSERT_TRUE((std::is_same<type, function>::value));
}

TEST(function_traits_test, type_preserves_pointer_arguments)
{
    using function = void(int *, char *);
    using type = typename function_traits<function>::type;
    ASSERT_TRUE((std::is_same<type, function>::value));
}

TEST(function_traits_test, type_preserves_const_pointer_arguments)
{
    using function = void(int const *, char const *);
    using type = typename function_traits<function>::type;
    ASSERT_TRUE((std::is_same<type, function>::value));
}

TEST(function_traits_test, can_prepend_argument)
{
    using type = typename prepend_argument<double, void(int)>::type;
    ASSERT_TRUE((std::is_same<type, void(double, int)>::value));
}

TEST(function_traits_test, can_prepend_argument_to_void_function)
{
    using type = typename prepend_argument<double, void()>::type;
    ASSERT_TRUE((std::is_same<type, void(double)>::value));
}

TEST(function_traits_test, can_convert_to_const_ref_args_from_values)
{
    using function = void(int, char);
    using type = typename const_ref_args<function>::type;
    using expected = void(int const &, char const &);

    ASSERT_TRUE((std::is_same<type, expected>::value));
}

TEST(function_traits_test, can_convert_to_const_ref_args_from_const_values)
{
    using function = void(int const, char const);
    using type = typename const_ref_args<function>::type;
    using expected = void(int const &, char const &);

    ASSERT_TRUE((std::is_same<type, expected>::value));
}

TEST(function_traits_test, can_convert_to_const_ref_args_from_refs)
{
    using function = void(int &, char &);
    using type = typename const_ref_args<function>::type;
    using expected = void(int const &, char const &);

    ASSERT_TRUE((std::is_same<type, expected>::value));
}

TEST(function_traits_test, can_convert_to_const_ref_args_from_const_refs)
{
    using function = void(int const &, char const &);
    using type = typename const_ref_args<function>::type;
    using expected = void(int const &, char const &);

    ASSERT_TRUE((std::is_same<type, expected>::value));
}

TEST(function_traits_test, can_convert_to_const_ref_args_from_pointers)
{
    using function = void(int *, char *);
    using type = typename const_ref_args<function>::type;
    using expected = void(int * const &, char * const &);

    ASSERT_TRUE((std::is_same<type, expected>::value));
}

} } }
