/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
}

TEST_CASE("meta/meta_types/array_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::array_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("int[]") {
        const meta::array_type type = meta::resolve_type<int[]>();
        REQUIRE(type);

        CHECK(type.get_flags() == (meta::array_flags::is_unbounded));

        CHECK(type.get_extent() == 0);
        CHECK(type.get_data_type() == meta::resolve_type<int>());
    }

    SUBCASE("const unsigned[42][21]") {
        const meta::array_type type = meta::resolve_type<const unsigned[42][21]>();
        REQUIRE(type);

        CHECK(type.get_flags() == (meta::array_flags::is_bounded));

        CHECK(type.get_extent() == 42);
        CHECK(type.get_data_type() == meta::resolve_type<const unsigned[21]>());
    }
}
