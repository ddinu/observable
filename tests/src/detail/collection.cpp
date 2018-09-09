#include <array>
#include <atomic>
#include <type_traits>
#include <thread>
#include <vector>
#include <unordered_set>
#include <catch/catch.hpp>
#include <observable/detail/collection.hpp>

namespace observable { namespace detail { namespace test {

TEST_CASE("collection/default constructor", "[collection]")
{
    SECTION("collections are default-constructible")
    {
        REQUIRE(std::is_nothrow_default_constructible<collection<int>>::value);
    }

    SECTION("default-constructed collection is empty")
    {
        REQUIRE(collection<int> { }.empty());
    }
}

TEST_CASE("collection/copy operations", "[collection]")
{
    SECTION("collections are not copy-constructible")
    {
        REQUIRE_FALSE(std::is_copy_constructible<collection<int>>::value);
    }

    SECTION("collections are not copy-assignable")
    {
        REQUIRE_FALSE(std::is_copy_assignable<collection<int>>::value);
    }
}

TEST_CASE("collection/move operations", "[collection]")
{
    SECTION("collections are not move-constructible")
    {
        REQUIRE_FALSE(std::is_move_constructible<collection<int>>::value);
    }

    SECTION("collections are not move-assignable")
    {
        REQUIRE_FALSE(std::is_move_assignable<collection<int>>::value);
    }
}

TEST_CASE("collection/insert", "[collection]")
{
    collection<int> col;

    SECTION("can insert items")
    {
        col.insert(5);
    }

    SECTION("collection is not empty after insert")
    {
        col.insert(5);
        REQUIRE_FALSE(col.empty());
    }
}

TEST_CASE("collection/apply", "[collection]")
{
    collection<int> col;

    SECTION("can apply functor to elements")
    {
        auto call_count = 0;

        col.insert(5);
        col.insert(6);
        col.apply([&](auto) { ++call_count; });

        REQUIRE(call_count == 2);
    }

    SECTION("apply does nothing for empty collection")
    {
        auto call_count = 0;
        col.apply([&](auto) { ++call_count; });

        REQUIRE(call_count == 0);
    }

    SECTION("elements are passed to the apply functor")
    {
        auto result = 0;

        col.insert(11);
        col.insert(7);
        col.apply([&](auto v) { result += v; });

        REQUIRE(result == 11 + 7);
    }

    SECTION("apply is nothrow for nothrow functor")
    {
        auto fun = [](auto) noexcept(true) { };

        REQUIRE(noexcept(col.apply(fun)));
    }

    SECTION("apply is not nothrow for throwing functor")
    {
        auto fun = [](auto) noexcept(false) { };

        REQUIRE_FALSE(noexcept(col.apply(fun)));
    }
}

TEST_CASE("collection/remove", "[collection]")
{
    SECTION("can remove elements")
    {
        collection<int> col;
        auto call_count = 0;
        auto id = col.insert(5);
        auto success = col.remove(id);

        REQUIRE(success);
        REQUIRE(col.empty());

        col.apply([&](auto) { ++call_count; });

        REQUIRE(call_count == 0);
    }

    SECTION("remove is nothrow")
    {
        collection<int> col;
        REQUIRE(noexcept(col.remove(collection<int>::id { })));
    }
}

TEST_CASE("collection/mutations during apply", "[collection]")
{
    SECTION("will not call apply for a removed element that has not been applied")
    {
        collection<unsigned int> col;

        auto ids = std::array<collection<unsigned int>::id, 3> { };
        for(auto i = 0u; i < ids.size(); ++i)
            ids[i] = col.insert(i);

        auto call_count = 0;
        col.apply([&](auto j) {
            for(auto i = 0u; i < ids.size(); ++i)
                if(i != j)
                    col.remove(ids[i]);
            ++call_count;
        });

        REQUIRE(call_count == 1);
    }

    SECTION("can remove already applied element")
    {
        collection<unsigned int> col;

        auto ids = std::array<collection<unsigned int>::id, 3> { };
        for(auto i = 0u; i < ids.size(); ++i)
            ids[i] = col.insert(i);

        col.apply([&](auto i) { col.remove(ids[i]); });

        REQUIRE(col.empty());
    }

    SECTION("can insert element during apply call")
    {
        collection<int> col;
        col.insert(3);

        auto insert_count = 0;
        col.apply([&](auto) {
            if(insert_count == 0)
                col.insert(7);
            ++insert_count;
        });

        auto sum = 0;
        col.apply([&](auto v) { sum += v; });

        REQUIRE(sum == 3 + 7);
    }
}

TEST_CASE("collection/concurrent mutations", "[collection]")
{
    SECTION("can insert elements in parallel")
    {
        // This test seems evil, but it fails with a decent probability.
        collection<int> col;

        auto ts = std::vector<std::thread> { };
        auto ref_sum = 0;
        std::atomic<bool> wait { true };

        for(auto i = 1; i <= 8; ++i)
        {
            ref_sum += i;
            ts.emplace_back([&, i]() {
                while(wait)
                    ;
                col.insert(i);
            });
        }

        wait = false;
        for(auto && t : ts)
            t.join();

        auto ref_els = std::unordered_set<int> { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto els = std::unordered_set<int> { };
        col.apply([&](auto i) { els.insert(i); });

        REQUIRE(ref_els == els);
    }

    SECTION("can remove elements in parallel")
    {
        // This test seems evil, but it fails with a decent probability.
        collection<unsigned int> col;

        auto ts = std::vector<std::thread> { };
        std::array<std::atomic<collection<unsigned int>::id>, 8> ids;
        std::atomic<bool> wait { true };

        for(auto i = 0u; i < ids.size(); ++i)
        {
            ids[i] = col.insert(i);
            ts.emplace_back([&, i]() {
                while(wait)
                    ;
                col.remove(ids[i]);
            });
        }

        wait = false;
        for(auto && t : ts)
            t.join();

        REQUIRE(col.empty());
    }

    SECTION("can insert and remove in parallel")
    {
        // This test seems evil, but it fails with a decent probability.
        collection<unsigned int> col;

        auto ts = std::vector<std::thread> { };
        std::array<
            std::array<std::atomic<collection<unsigned int>::id>, 8>,
            4> ids;
        std::atomic<bool> wait { true };

        for(auto j = 0u; j < 4u; ++j)
        {
            ts.emplace_back([&, j]() {
                while(wait)
                    ;
                for(auto i = 0u; i < 8u; ++i)
                    ids[j][i] = col.insert(i);

                for(auto i = 0u; i < 8u; ++i)
                    col.remove(ids[j][i]);
            });
        }

        wait = false;
        for(auto && t : ts)
            t.join();

        REQUIRE(col.empty());
    }

    SECTION("can remove same node in parallel")
    {
        collection<unsigned int> col;

        std::array<std::atomic<collection<unsigned int>::id>, 3> ids;
        for(auto i = 0u; i < ids.size(); ++i)
            ids[i] = col.insert(i);

        std::atomic<bool> wait { true };
        auto ts = std::vector<std::thread> { };
        for(auto j = 0; j < 8; ++j)
            ts.emplace_back([&]() {
                while(wait)
                    ;
                for(auto i = 0u; i < ids.size(); ++i)
                    col.remove(ids[i]);
            });

        wait = false;
        for(auto && t : ts)
            t.join();

        REQUIRE(col.empty());
    }
}

} } }
