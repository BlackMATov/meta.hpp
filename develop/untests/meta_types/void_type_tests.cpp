/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

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
    }

    SUBCASE("void*") {
        void* ptr{};
        const meta::pointer_type ptr_type = meta::resolve_type(ptr);
        CHECK(ptr_type == meta::resolve_type<void*>());
        CHECK_FALSE(ptr_type.get_flags().has(meta::pointer_flags::is_readonly));
        CHECK(ptr_type.get_data_type() == meta::resolve_type<void>());
    }

    SUBCASE("const void*") {
        const void* ptr{};
        const meta::pointer_type ptr_type = meta::resolve_type(ptr);
        CHECK(ptr_type == meta::resolve_type<const void*>());
        CHECK(ptr_type.get_flags().has(meta::pointer_flags::is_readonly));
        CHECK(ptr_type.get_data_type() == meta::resolve_type<void>());
    }
}
