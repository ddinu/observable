#define _USE_MATH_DEFINES
#include <cassert>
#include <cmath>
#include <observable/observable.hpp>
#include "gtest.h"

using namespace std::string_literals;

TEST(example_tests, using_expressions_simple_expressions)
{
    // BEGIN EXAMPLE CODE

    //int main()
    {
        auto radius = observable::value<double> { 5 };
        auto circumference = observe(2 * M_PI * radius);

        assert(fabs(circumference.get() - 2 * M_PI * 5) < 0.001);

        radius = 7;

        assert(fabs(circumference.get() - 2 * M_PI * 7) < 0.001);
    }

    // END EXAMPLE CODE
}

TEST(example_tests, using_expressions_with_updater)
{
    // BEGIN EXAMPLE CODE

    //int main()
    {
        auto radius = observable::value<double> { 5 };

        auto updater = observable::updater { };
        auto circumference = observe(updater, 2 * M_PI * radius);

        assert(fabs(circumference.get() - 2 * M_PI * 5) < 0.001);

        radius = 7;
        assert(fabs(circumference.get() - 2 * M_PI * 5) < 0.001);

        updater.update_all();
        assert(fabs(circumference.get() - 2 * M_PI * 7) < 0.001);
    }

    // END EXAMPLE CODE
}

TEST(example_tests, using_expressions_predefined_filters)
{
    // BEGIN EXAMPLE CODE

    //int main()
    {
        auto radius = observable::value<double> { 5 };
        auto area = observe(M_PI * pow(radius, 2));
        auto is_large = observe(select(area > 100, true, false));

        assert(fabs(area.get() - M_PI * std::pow(5, 2)) < 0.001);
        assert(is_large.get() == false);

        radius = 70;

        assert(fabs(area.get() - M_PI * std::pow(70, 2)) < 0.001);
        assert(is_large.get() == true);
    }

    // END EXAMPLE CODE
}

namespace {
    double square_(double val) { return std::pow(val, 2); }
    OBSERVABLE_ADAPT_FILTER(square, square_)
}

TEST(example_tests, using_expressions_user_filters)
{
    // BEGIN EXAMPLE CODE

    //int main()
    {
        auto radius = observable::value<double> { 5 };
        auto area = observe(M_PI * square(radius));

        assert(fabs(area.get() - M_PI * std::pow(5, 2)) < 0.001);

        radius = 70;

        assert(fabs(area.get() - M_PI * std::pow(70, 2)) < 0.001);
    }

    // END EXAMPLE CODE
}
