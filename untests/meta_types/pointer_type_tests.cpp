/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
}

TEST_CASE("meta/meta_types/pointer_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::pointer_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("int*") {
        const meta::pointer_type type = meta::resolve_type<int*>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<int*>().get_id());
        CHECK(type.get_data_type() == meta::resolve_type<int>());
    }

    SUBCASE("const int* const") {
        const meta::pointer_type type = meta::resolve_type<const int* const>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<const int*>().get_id());
        CHECK(type.get_data_type() == meta::resolve_type<const int>());
    }

    SUBCASE("int**") {
        const meta::pointer_type type = meta::resolve_type<int**>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<int**>().get_id());
        CHECK(type.get_data_type() == meta::resolve_type<int*>());
    }

    SUBCASE("const int** const") {
        const meta::pointer_type type = meta::resolve_type<const int** const>();
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type<const int**>().get_id());
        CHECK(type.get_data_type() == meta::resolve_type<const int*>());
    }
}
