/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
}

TEST_CASE("meta/meta_types/number_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::number_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("int") {
        const meta::number_type type = meta::resolve_type<int>();
        REQUIRE(type);

        CHECK(type.get_size() == sizeof(int));
        CHECK(type.get_flags() == (
            meta::number_flags::is_signed |
            meta::number_flags::is_integral));
    }

    SUBCASE("const float") {
        const meta::number_type type = meta::resolve_type<const float>();
        REQUIRE(type);

        CHECK(type.get_size() == sizeof(float));
        CHECK(type.get_flags() == (
            meta::number_flags::is_signed |
            meta::number_flags::is_floating_point));
    }

    SUBCASE("const unsigned") {
        const meta::number_type type = meta::resolve_type<const unsigned>();
        REQUIRE(type);

        CHECK(type.get_size() == sizeof(unsigned));
        CHECK(type.get_flags() == (
            meta::number_flags::is_unsigned |
            meta::number_flags::is_integral));
    }
}