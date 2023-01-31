/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_base/hashed_string") {
    namespace meta = meta_hpp;
    using meta::detail::hashed_string;

    SUBCASE("ctor/0") {
        constexpr hashed_string hs{};
        static_assert(hs == hashed_string{""});
        static_assert(hs.get_hash() == hashed_string{""}.get_hash());

        CHECK(hs == hashed_string{std::string{}});
        CHECK(hs == hashed_string{std::string_view{}});
    }

    SUBCASE("ctor/1") {
        constexpr hashed_string hs{"hello"};
        static_assert(hs.get_hash() == meta::detail::fnv1a_hash("hello"));

        CHECK(hs == hashed_string{std::string{"hello"}});
        CHECK(hs == hashed_string{std::string_view{"hello"}});
    }

    SUBCASE("copy_ctor") {
        constexpr hashed_string hs{"hello"};
        constexpr hashed_string hs2{hs};
        static_assert(hs == hs2);
        static_assert(hs.get_hash() == hs2.get_hash());
    }

    SUBCASE("move_ctor") {
        constexpr hashed_string hs{"hello"};
        constexpr hashed_string hs2{std::move(hs)};
        static_assert(hs == hs2);
        static_assert(hs.get_hash() == hs2.get_hash());
    }

    SUBCASE("operator=/copy") {
        constexpr hashed_string hs{"hello"};
        constexpr hashed_string hs2 = [&hs](){
            hashed_string r;
            r = hs;
            return r;
        }();
        static_assert(hs == hs2);
        static_assert(hs.get_hash() == hs2.get_hash());
    }

    SUBCASE("operator=/move") {
        constexpr hashed_string hs{"hello"};
        constexpr hashed_string hs2 = [&hs](){
            hashed_string r;
            r = std::move(hs);
            return r;
        }();
        static_assert(hs == hs2);
        static_assert(hs.get_hash() == hs2.get_hash());
    }

    SUBCASE("get_hash") {
        constexpr hashed_string hs1{"hello"};
        constexpr hashed_string hs2{"world"};
        static_assert(hs1.get_hash() == hashed_string{"hello"}.get_hash());
        static_assert(hs1.get_hash() != hs2.get_hash());
        CHECK(std::hash<hashed_string>{}(hs1) == hs1.get_hash());
    }

    SUBCASE("operator<") {
        constexpr hashed_string hs1{"hello"};
        constexpr hashed_string hs2{"hello"};
        static_assert(!(hs1 < hs2) && !(hs2 < hs1));
        static_assert(hs1 < hashed_string{"world"} || hashed_string{"world"} < hs1);
    }

    SUBCASE("operator==") {
        constexpr hashed_string hs1{"hello"};
        static_assert(hs1 == hashed_string{"hello"});
        static_assert(hs1 != hashed_string{"world"});
    }

    SUBCASE("swap") {
        hashed_string hs1{"hello"};
        hashed_string hs2{"world"};
        swap(hs1, hs2);
        CHECK(hs1 == hashed_string{"world"});
        CHECK(hs2 == hashed_string{"hello"});
    }
}
