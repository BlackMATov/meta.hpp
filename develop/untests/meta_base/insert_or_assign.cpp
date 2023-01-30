/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct movable_value {
        int key{};
        int data{};

        movable_value(int k, int d) : key{k}, data{d} {}

        [[maybe_unused]] movable_value(movable_value&&) = default;
        [[maybe_unused]] movable_value& operator=(movable_value&&) = default;

        movable_value(const movable_value&) = delete;
        movable_value& operator=(const movable_value&) = delete;

        [[maybe_unused]]
        friend bool operator<(const movable_value& l, const movable_value& r) noexcept {
            return l.key < r.key;
        }

        [[maybe_unused]]
        friend bool operator==(const movable_value& l, const movable_value& r) noexcept {
            return l.key == r.key && l.data == r.data;
        }
    };

    struct copyable_value {
        int key{};
        int data{};

        [[maybe_unused]]
        friend bool operator<(const copyable_value& l, const copyable_value& r) noexcept {
            return l.key < r.key;
        }

        [[maybe_unused]]
        friend bool operator==(const copyable_value& l, const copyable_value& r) noexcept {
            return l.key == r.key && l.data == r.data;
        }
    };
}

TEST_CASE("meta/meta_base/insert_or_assign") {
    namespace meta = meta_hpp;
    using meta::detail::insert_or_assign;

    SUBCASE("movable_value/insert") {
        std::set<movable_value, std::less<>> s;
        s.insert({1, 10});
        s.insert({3, 30});
        CHECK(*insert_or_assign(s, movable_value{2, 20}) == movable_value{2, 20});
        {
            std::set<movable_value, std::less<>> s2;
            s2.insert({1, 10});
            s2.insert({2, 20});
            s2.insert({3, 30});
            CHECK(s == s2);
        }
    }

    SUBCASE("movable_value/replace") {
        std::set<movable_value, std::less<>> s;
        s.insert({1, 10});
        s.insert({3, 30});
        CHECK(*insert_or_assign(s, movable_value{3, 42}) == movable_value{3, 42});
        {
            std::set<movable_value, std::less<>> s2;
            s2.insert({1, 10});
            s2.insert({3, 42});
            CHECK(s == s2);
        }
    }

    SUBCASE("copyable_value/insert/0") {
        std::set<copyable_value, std::less<>> s{{1, 10}, {3, 30}};
        CHECK(*insert_or_assign(s, copyable_value{2, 20}) == copyable_value{2, 20});
        CHECK(s == std::set<copyable_value, std::less<>>{{1, 10}, {2, 20}, {3, 30}});
    }

    SUBCASE("copyable_value/insert/1") {
        std::set<copyable_value, std::less<>> s{{1, 10}, {3, 30}};
        copyable_value v{2, 20};
        CHECK(*insert_or_assign(s, v) == v);
        CHECK(s == std::set<copyable_value, std::less<>>{{1, 10}, {2, 20}, {3, 30}});
    }

    SUBCASE("copyable_value/replace/0") {
        std::set<copyable_value, std::less<>> s{{1, 10}, {3, 30}};
        CHECK(*insert_or_assign(s, copyable_value{3, 42}) == copyable_value{3, 42});
        CHECK(s == std::set<copyable_value, std::less<>>{{1, 10}, {3, 42}});
    }

    SUBCASE("copyable_value/replace/1") {
        std::set<copyable_value, std::less<>> s{{1, 10}, {3, 30}};
        copyable_value v{3, 42};
        CHECK(*insert_or_assign(s, v) == v);
        CHECK(s == std::set<copyable_value, std::less<>>{{1, 10}, {3, 42}});
    }
}
