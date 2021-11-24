/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
}

TEST_CASE("meta/meta_types/void_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::void_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("void") {
        const meta::void_type type = meta::resolve_type<void>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<void>().get_id());
        CHECK(type.get_flags() == meta::void_flags{});
    }
}
