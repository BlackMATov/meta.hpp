/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_types/type_id") {
    namespace meta = meta_hpp;

    SUBCASE("ctors/0") {
        const meta::type_id id;
        CHECK_FALSE(id);
        CHECK_FALSE(id.is_valid());
    }

    SUBCASE("ctors/1") {
        const meta::type_id int_id = meta::resolve_type<int>().get_id();
        const meta::type_id float_id = meta::resolve_type<float>().get_id();

        REQUIRE(int_id);
        REQUIRE(float_id);

        CHECK_FALSE(int_id == float_id);
        CHECK(int_id != float_id);
        CHECK((int_id < float_id || (float_id < int_id)));
    }

    SUBCASE("get_hash") {
        const meta::type_id int_id = meta::resolve_type<int>().get_id();
        const meta::type_id float_id = meta::resolve_type<float>().get_id();

        REQUIRE(int_id);
        REQUIRE(float_id);

        CHECK(meta::type_id{}.get_hash() == 0);
        CHECK_FALSE(int_id.get_hash() == float_id.get_hash());
        CHECK(int_id.get_hash() == meta::resolve_type<int>().get_id().get_hash());
    }
}
