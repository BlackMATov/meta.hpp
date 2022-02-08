/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
}

TEST_CASE("meta/meta_types/reference_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::reference_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("int&") {
        const meta::reference_type type = meta::resolve_type<int&>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<int&>().get_id());
        CHECK(type.get_flags() == (meta::reference_flags::is_lvalue));
        CHECK(type.get_data_type() == meta::resolve_type<int>());
    }

    SUBCASE("const int&") {
        const meta::reference_type type = meta::resolve_type<const int&>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<const int&>().get_id());
        CHECK(type.get_flags() == (meta::reference_flags::is_readonly | meta::reference_flags::is_lvalue));
        CHECK(type.get_data_type() == meta::resolve_type<int>());
    }

    SUBCASE("int&&") {
        const meta::reference_type type = meta::resolve_type<int&&>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<int&&>().get_id());
        CHECK(type.get_flags() == (meta::reference_flags::is_rvalue));
        CHECK(type.get_data_type() == meta::resolve_type<int>());
    }

    SUBCASE("const int&&") {
        const meta::reference_type type = meta::resolve_type<const int&&>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<const int&&>().get_id());
        CHECK(type.get_flags() == (meta::reference_flags::is_readonly | meta::reference_flags::is_rvalue));
        CHECK(type.get_data_type() == meta::resolve_type<int>());
    }
}
