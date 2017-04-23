#include <type_traits>
#include <observable/subject.hpp>
#include <observable/detail/type_traits.hpp>
#include "gtest.h"

namespace observable { namespace detail { namespace test {

TEST(type_traits_test, is_compatible_with_observer_is_true_for_compatible_function)
{
    auto compat = is_compatible_with_observer<
                        void(int),
                        subject<void(int)>::observer_type
                  >::value;
    ASSERT_TRUE(compat);
}

TEST(type_traits_test, is_compatible_with_observer_is_false_for_incompatible_function)
{
    auto compat = is_compatible_with_observer<
                        void(int &),
                        subject<void(int)>::observer_type
                  >::value;
    ASSERT_FALSE(compat);
}

TEST(type_traits_test, is_compatible_with_subject_is_true_for_compatible_function)
{
    auto compat = is_compatible_with_subject<void(int), subject<void(int)>>::value;
    ASSERT_TRUE(compat);
}

TEST(type_traits_test, is_compatible_with_subject_is_false_for_incompatible_function)
{
    auto compat = is_compatible_with_subject<void(int &), subject<void(int)>>::value;
    ASSERT_FALSE(compat);
}

} } }
