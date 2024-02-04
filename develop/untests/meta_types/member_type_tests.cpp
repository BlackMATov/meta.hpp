/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct clazz_1 {
        int int_member = 1;
        const int const_int_member = 2;
    };
}

TEST_CASE("meta/meta_types/member_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::member_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("traits") {
        using meta::detail::member_traits;

        static_assert(!member_traits<int clazz_1::*>::is_readonly);
        static_assert(member_traits<const int clazz_1::*>::is_readonly);

        static_assert(!member_traits<int clazz_1::*>::is_volatile);
        static_assert(member_traits<volatile int clazz_1::*>::is_volatile);

        static_assert(std::is_same_v<member_traits<int clazz_1::*>::value_type, int>);
        static_assert(std::is_same_v<member_traits<const int clazz_1::*>::value_type, int>);
        static_assert(std::is_same_v<member_traits<const volatile int clazz_1::*>::value_type, int>);

        static_assert(std::is_same_v<member_traits<int clazz_1::*>::cv_value_type, int>);
        static_assert(std::is_same_v<member_traits<const int clazz_1::*>::cv_value_type, const int>);
        static_assert(std::is_same_v<member_traits<const volatile int clazz_1::*>::cv_value_type, const volatile int>);
    }

    SUBCASE("int") {
        const meta::member_type type = meta::resolve_type(&clazz_1::int_member);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(&clazz_1::int_member).get_id());
        CHECK(type.get_flags() == meta::member_flags{});

        CHECK(type.get_owner_type() == meta::resolve_type<clazz_1>());
        CHECK(type.get_value_type() == meta::resolve_type<int>());
    }

    SUBCASE("const int") {
        const meta::member_type type = meta::resolve_type(&clazz_1::const_int_member);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(&clazz_1::const_int_member).get_id());
        CHECK(type.get_flags() == meta::member_flags::is_readonly);

        CHECK(type.get_owner_type() == meta::resolve_type<clazz_1>());
        CHECK(type.get_value_type() == meta::resolve_type<int>());
    }
}
