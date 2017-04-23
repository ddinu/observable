#include <type_traits>
#include <observable/value.hpp>
#include "gtest.h"

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

    auto get() const -> int override { return v_; }

    void set_value_notifier(std::function<void(int &&)> const & f) { fun_ = f; }

    void set(int v) { v_ = v; fun_(std::move(v_)); }

private:
    std::function<void(int &&)> fun_;
    int v_;
};

TEST(value_test, is_default_constructible)
{
    value<int> { };

    ASSERT_TRUE(std::is_default_constructible<value<int>>::value);
}

TEST(value_test, is_not_copy_constructible)
{
    ASSERT_FALSE(std::is_copy_constructible<value<int>>::value);
}

TEST(value_test, is_not_copy_assignable)
{
    ASSERT_FALSE(std::is_copy_assignable<value<int>>::value);
}

TEST(value_test, is_nothrow_move_constructible)
{
    ASSERT_TRUE(std::is_nothrow_move_constructible<value<int>>::value);
}

TEST(value_test, is_not_nothrow_move_constructible_if_value_type_throws)
{
    ASSERT_TRUE(std::is_move_constructible<value<throwing_value>>::value);
    ASSERT_FALSE(std::is_nothrow_move_constructible<value<throwing_value>>::value);
}

TEST(value_test, is_nothrow_move_assignable)
{
    ASSERT_TRUE(std::is_move_assignable<value<int>>::value);
    ASSERT_TRUE(std::is_nothrow_move_assignable<value<int>>::value);
}

TEST(value_test, is_not_nothrow_move_assignable_if_value_type_throws)
{
    ASSERT_TRUE(std::is_move_assignable<value<throwing_value>>::value);
    ASSERT_FALSE(std::is_nothrow_move_assignable<value<throwing_value>>::value);
}

TEST(value_test, can_create_initialized_value)
{
    value<int> { 123 };
}

TEST(value_test, can_get_value)
{
    auto val = value<int> { 123 };

    ASSERT_EQ(val.get(), 123);
}

TEST(value_test, getter_is_nothrow)
{
    auto val = value<int> { };

    ASSERT_TRUE(noexcept(val.get()));
}

TEST(value_test, can_convert_to_value_type)
{
    auto val = value<int> { 123 };
    auto v = static_cast<int>(val);

    ASSERT_EQ(v, 123);
}

TEST(value_test, conversion_operator_is_nothrow)
{
    auto val = value<int> { };

    ASSERT_TRUE(noexcept(static_cast<int>(val)));
}

TEST(value_test, can_change_value_with_no_subscribed_observers)
{
    auto val = value<int> { 5 };
    val.set(7);

    ASSERT_EQ(7, val.get());
}

TEST(value_test, can_subscribe_to_value_changes)
{
    auto call_count = 0;

    auto val = value<int> { 123 };
    val.subscribe([&]() { ++call_count; }).release();
    val.set(1234);

    ASSERT_EQ(call_count, 1);
}

TEST(value_test, can_subscribe_to_value_changes_and_get_value)
{
    struct p { int a; };

    auto call_count = 0;
    auto v = p { };

    auto val = value<p> { };
    val.subscribe([&](auto new_v) { ++call_count; v = new_v; }).release();
    val.set(p { 1234 });

    ASSERT_EQ(call_count, 1);
    ASSERT_EQ(v.a, 1234);
}

TEST(value_test, setting_same_value_does_not_trigger_subscribers)
{
    auto call_count = 0;

    auto val = value<int> { 123 };
    val.subscribe([&]() { ++call_count; });
    val.subscribe([&](int) { ++call_count; });
    val.set(123);

    ASSERT_EQ(call_count, 0);
}

TEST(value_test, move_constructed_value_is_correct)
{
    auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
    auto moved_val = value<std::unique_ptr<int>> { std::move(val) };

    ASSERT_EQ(*moved_val.get(), 123);
}

TEST(value_test, move_assigned_value_is_correct)
{
    auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
    auto moved_val = std::move(val);

    ASSERT_EQ(*moved_val.get(), 123);
}

TEST(value_test, move_constructed_value_keeps_subscribers)
{
    auto call_count = 0;

    auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
    val.subscribe([&]() { ++call_count; }).release();

    auto moved_val = value<std::unique_ptr<int>> { std::move(val) };
    moved_val.set(std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

TEST(value_test, move_assigned_value_keeps_subscribers)
{
    auto call_count = 0;

    auto val = value<std::unique_ptr<int>> { std::make_unique<int>(123) };
    val.subscribe([&]() { ++call_count; }).release();

    auto moved_val = std::move(val);
    moved_val.set(std::make_unique<int>(1234));

    ASSERT_EQ(call_count, 1);
}

TEST(value_test, can_use_value_enclosed_in_class)
{
    struct mock
    {
        value<int, mock> val { 0 };
        void set_value(int v) { val = v; }
    } foo;

    auto called = false;
    foo.val.subscribe([&]() { called = true; }).release();
    foo.set_value(5);

    ASSERT_EQ(foo.val.get(), 5);
    ASSERT_TRUE(called);
}

TEST(value_test, can_create_value_with_updater)
{
    auto val = value<int> { std::make_unique<mock_updater>(5) };

    ASSERT_EQ(5, val.get());
}

TEST(value_test, value_is_updated_by_the_updater)
{
    auto updater = new mock_updater { 5 };
    auto val = value<int> { std::unique_ptr<mock_updater> { updater } };
    updater->set(7);

    ASSERT_EQ(7, val.get());
}

TEST(value_test, value_with_updater_throws_on_set)
{
    auto val = value<int> { std::make_unique<mock_updater>(5) };

    ASSERT_THROW({ val = 7; }, readonly_value);
}

TEST(value_test, change_notification_is_triggered_by_the_updater)
{
    auto updater = new mock_updater { 5 };
    auto val = value<int> { std::unique_ptr<mock_updater> { updater } };

    auto call_count = 0;
    val.subscribe([&]() { ++call_count; }).release();

    updater->set(7);

    ASSERT_EQ(1, call_count);
}

TEST(value_test, value_is_updated_by_updater_after_move)
{
    auto updater = new mock_updater { 5 };
    auto val = value<int> { std::unique_ptr<mock_updater> { updater } };

    auto new_val = std::move(val);
    updater->set(7);

    ASSERT_EQ(7, new_val.get());
}

TEST(value_test, can_create_value_with_non_equality_comparable_type)
{
    struct dummy { };
    auto v = value<dummy> { };
    v.get();
}

TEST(value_test, value_with_non_equality_comparable_type_always_triggers_change)
{
    struct dummy { };
    auto v = value<dummy> { };

    auto call_count = 0;
    v.subscribe([&]() { ++call_count; });
    v.subscribe([&](auto &&) { ++call_count; });
    v = dummy { };

    ASSERT_EQ(2, call_count);
}

TEST(value_test, can_create_value_with_custom_equality_comparator)
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

    ASSERT_EQ(0, call_count);
    ASSERT_EQ(5, v.get());
}

TEST(value_test, moved_value_keeps_custom_equality_comparator)
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

    ASSERT_EQ(0, call_count);
    ASSERT_EQ(5, moved.get());
}

} }
