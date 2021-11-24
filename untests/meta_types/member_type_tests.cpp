/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

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
        CHECK(type.get_flags() == meta::member_flags{});

        CHECK(type.get_owner_type() == meta::resolve_type<clazz_1>());
        CHECK(type.get_value_type() == meta::resolve_type<int>());
    }
}
