/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        ivec2& add(const ivec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }
    };
}

TEST_CASE("meta/meta_indices/ops/_") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y)
        .method_("add", &ivec2::add);
}

TEST_CASE("meta/meta_indices/ops") {
    namespace meta = meta_hpp;

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    const meta::member ivec2_x = ivec2_type.get_member("x");
    REQUIRE(ivec2_x);

    const meta::member ivec2_y = ivec2_type.get_member("y");
    REQUIRE(ivec2_y);

    const meta::method ivec2_add = ivec2_type.get_method("add");
    REQUIRE(ivec2_add);

    SUBCASE("get_type") {
        CHECK(ivec2_x.get_index().get_type() == meta::resolve_type<int ivec2::*>());
        CHECK(ivec2_add.get_index().get_type() == meta::resolve_type<ivec2& (ivec2::*)(const ivec2&)>());
    }

    SUBCASE("get_name") {
        {
            meta::member_index ivec2_x_index = ivec2_x.get_index();
            CHECK(ivec2_x_index.get_name() == "x");
            CHECK(std::move(ivec2_x_index).get_name() == "x");
        }
        {
            meta::method_index ivec2_add_index = ivec2_add.get_index();
            CHECK(ivec2_add_index.get_name() == "add");
            CHECK(std::move(ivec2_add_index).get_name() == "add");
        }
    }
}
