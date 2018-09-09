#include <type_traits>
#include <catch/catch.hpp>
#include <observable/value.hpp>

namespace observable { namespace test {

struct throwing_value
{
    throwing_value() noexcept(false) { }
    throwing_value(throwing_value const &) noexcept(false) { }
    auto operator=(throwing_value const &) noexcept(false) { }
    throwing_value(throwing_value &&) noexcept(false) { }
    auto operator=(throwing_value &&) noexcept(false) { }
};

struct mock_updater : value_updater<int>
{
    mock_updater(int initial) : v_ { initial }
    { }

    auto get() const -> int override;

    void set_value_notifier(std::function<void(int &&)> const & f) override;

    void set(int v) { v_ = v; fun_(std::move(v_)); }

private:
    std::function<void(int &&)> fun_;
    int v_;
};

auto mock_updater::get() const -> int
{
    return v_;
}

void mock_updater::set_value_notifier(std::function<void(int &&)> const & f)
{
    fun_ = f;
}

TEST_CASE("value/basic value creation", "[value]")
{
    SECTION("values are default-constructible")
    {
        value<int> { };

        REQUIRE(std::is_default_constructible<value<int>>::value);
    }

    SECTION("can create initialized value")
    {
        value<int> { 123 };
    }
}

TEST_CASE("value/copying", "[value]")
{
    SECTION("values are not copy-constructible")
    {
        REQUIRE_FALSE(std::is_copy_constructible<value<int>>::value);
    }

    SECTION("values are not copy-assignable")
    {
        REQUIRE_FALSE(std::is_copy_assignable<value<int>>::value);
    }
}

TEST_CASE("value/moving", "[value]")
{
    SECTION("values are nothrow move-constructible")
    {
        REQUIRE(std::is_nothrow_move_constructible<value<int>>::value);
    }

    SECTION("values are not nothrow move-constructible if value type move constructor throws")
    {
        REQUIRE(std::is_move_constructible<value<throwing_value>>::value);
        REQUIRE_FALSE(std::is_nothrow_move_constructible<value<throwing_value>>::value);
    }

    SECTION("values are nothrow move-assignable")
    {
        REQUIRE(std::is_move_assignable<value<int>>::value);
        REQUIRE(std::is_nothrow_move_assignable<value<int>>::value);
    }

    SECTION("values are not nothrow move-assignable if value type move assignment operator throws")
    {
        REQUIRE(std::is_move_assignable<value<throwing_value>>::value);
        REQUIRE_FALSE(std::is_nothrow_move_assignable<value<throwing_value>>::value);
    }

    SECTION("move constructed value is correct")
    {
        auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
        auto moved_val = value<std::unique_ptr<int>> { std::move(val) };

        REQUIRE(*moved_val.get() == 123);
    }

    SECTION("move assigned value is correct")
    {
        auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
        auto moved_val = std::move(val);

        REQUIRE(*moved_val.get() == 123);
    }

    SECTION("move constructed value keeps subscribers")
    {
        auto call_count = 0;

        auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
        val.subscribe([&]() { ++call_count; }).release();

        auto moved_val = value<std::unique_ptr<int>> { std::move(val) };
        moved_val.set(std::make_unique<int>(1234));

        REQUIRE(call_count == 1);
    }

    SECTION("move assigned value keeps subscribers")
    {
        auto call_count = 0;

        auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
        val.subscribe([&]() { ++call_count; }).release();

        auto moved_val = std::move(val);
        moved_val.set(std::make_unique<int>(1234));

        REQUIRE(call_count == 1);
    }
}

TEST_CASE("value/value getter", "[value]")
{
    SECTION("can get value")
    {
        auto val = value<int> { 123 };
        REQUIRE(val.get() == 123);
    }

    SECTION("getter is nothrow")
    {
        auto val = value<int> { };
        REQUIRE(noexcept(val.get()));
    }
}

TEST_CASE("value/conversions", "[value]")
{
    SECTION("can convert to value_type")
    {
        auto val = value<int> { 123 };
        auto v = static_cast<int>(val);
        REQUIRE(v == 123);
    }

    SECTION("conversion operator is nothrow")
    {
        auto val = value<int> { };
        REQUIRE(noexcept(static_cast<int>(val)));
    }
}

TEST_CASE("value/subscribing", "[value]")
{
    SECTION("can change value with no subscribed observers")
    {
        auto val = value<int> { 5 };
        val.set(7);

        REQUIRE(val.get() == 7);
    }

    SECTION("can subscribe to value changes")
    {
        auto call_count = 0;

        auto val = value<int> { 123 };
        val.subscribe([&]() { ++call_count; }).release();
        val.set(1234);

        REQUIRE(call_count == 1);
    }

    SECTION("can subscribe to value changes on const values")
    {
        auto call_count = 0;

        auto val = value<int> { 123 };
        auto const & const_val = val;
        const_val.subscribe([&]() { ++call_count; });
        val.set(1234);

        REQUIRE(call_count == 1);
    }

    SECTION("can subscribe to value changes and get value")
    {
        struct p { int a; };

        auto call_count = 0;
        auto v = p { };

        auto val = value<p> { };
        val.subscribe([&](auto new_v) { ++call_count; v = new_v; }).release();
        val.set(p { 1234 });

        REQUIRE(call_count == 1);
        REQUIRE(v.a == 1234);
    }

    SECTION("setting same value does not trigger subscribers")
    {
        auto call_count = 0;

        auto val = value<int> { 123 };
        val.subscribe([&]() { ++call_count; });
        val.subscribe([&](int) { ++call_count; });
        val.set(123);

        REQUIRE(call_count == 0);
    }

    SECTION("can subscribe and immediately call observer")
    {
        auto v = value<int> { 5 };

        auto call_count = 0;
        auto sub = v.subscribe_and_call([&]() { ++call_count; });

        REQUIRE(call_count == 1);
    }

    SECTION("immediately called observer receives the current value")
    {
        auto val = value<int> { 5 };

        auto call_value = 3;
        auto sub = val.subscribe_and_call([&](int v) { call_value = v; });

        REQUIRE(call_value == 5);
    }
}

TEST_CASE("value/can use value enclosed in class", "[value]")
{
    struct mock
    {
        value<int, mock> val { 0 };
        void set_value(int v) { val = v; }
    } foo;

    auto called = false;
    foo.val.subscribe([&]() { called = true; }).release();
    foo.set_value(5);

    REQUIRE(foo.val.get() == 5);
    REQUIRE(called);
}

TEST_CASE("value/value updaters", "[value]")
{
    SECTION("can create value with updater")
    {
        auto val = value<int> { std::make_unique<mock_updater>(5) };

        REQUIRE(val.get() == 5);
    }

    SECTION("value is updated by the updater")
    {
        auto updater = new mock_updater { 5 };
        auto val = value<int> { std::unique_ptr<mock_updater> { updater } };
        updater->set(7);

        REQUIRE(val.get() == 7);
    }

    SECTION("value with updater throws on set")
    {
        auto val = value<int> { std::make_unique<mock_updater>(5) };

        REQUIRE_THROWS_AS((val = 7), readonly_value);
    }

    SECTION("change notification is triggered by the updater")
    {
        auto updater = new mock_updater { 5 };
        auto val = value<int> { std::unique_ptr<mock_updater> { updater } };

        auto call_count = 0;
        val.subscribe([&]() { ++call_count; }).release();

        updater->set(7);

        REQUIRE(call_count == 1);
    }

    SECTION("value is updated by updater after move")
    {
        auto updater = new mock_updater { 5 };
        auto val = value<int> { std::unique_ptr<mock_updater> { updater } };

        auto new_val = std::move(val);
        updater->set(7);

        REQUIRE(new_val.get() == 7);
    }
}

TEST_CASE("value/equality comparators", "[value]")
{
    SECTION("can create value with non equality-comparable type")
    {
        struct dummy { };
        auto v = value<dummy> { };
        v.get();
    }

    SECTION("value with non equality comparable type always triggers change")
    {
        struct dummy { };
        auto v = value<dummy> { };

        auto call_count = 0;
        v.subscribe([&]() { ++call_count; });
        v.subscribe([&](auto &&) { ++call_count; });
        v = dummy { };

        REQUIRE(call_count == 2);
    }

    SECTION("can create value with custom equality comparator")
    {
        auto v = value<int> {
                        5,
                        [](auto a, auto b) {
                            return std::abs(a) == std::abs(b);
                        }
                 };

        auto call_count = 0u;
        v.subscribe([&]() { ++call_count; });

        v = 5;
        v = -5;

        REQUIRE(call_count == 0);
        REQUIRE(v.get() == 5);
    }

    SECTION("moved value keeps custom equality comparator")
    {
        auto v = value<int> {
            5,
            [](auto a, auto b) {
                return std::abs(a) == std::abs(b);
            }
        };

        auto moved = std::move(v);

        auto call_count = 0u;
        moved.subscribe([&]() { ++call_count; });

        moved = 5;
        moved = -5;

        REQUIRE(call_count == 0);
        REQUIRE(moved.get() == 5);
    }
}

} }
