#include <type_traits>
#include <catch/catch.hpp>
#include <observable/subject.hpp>
#include <observable/detail/type_traits.hpp>

namespace observable { namespace detail { namespace test {

TEST_CASE("type_traits/is_compatible_with_observer", "[type_traits]")
{
    SECTION("is_compatible_with_observer is true for compatible function")
    {
        auto compat = is_compatible_with_observer<
                            void(int),
                            subject<void(int)>::observer_type
                      >::value;
        REQUIRE(compat);
    }

    SECTION("is_compatible_with_observer is false for incompatible function")
    {
        auto compat = is_compatible_with_observer<
                            void(int &),
                            subject<void(int)>::observer_type
                      >::value;
        REQUIRE_FALSE(compat);
    }
}

TEST_CASE("type_traits/is_compatible_with_subject", "[type_traits]")
{
    SECTION("is_compatible_with_subject is true for compatible function")
    {
        auto compat = is_compatible_with_subject<void(int), subject<void(int)>>::value;
        REQUIRE(compat);
    }

    SECTION("is_compatible_with_subject is false for incompatible function")
    {
        auto compat = is_compatible_with_subject<void(int &), subject<void(int)>>::value;
        REQUIRE_FALSE(compat);
    }
}

} } }
