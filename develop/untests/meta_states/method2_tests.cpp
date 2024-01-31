/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
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

TEST_CASE("meta/meta_states/method2") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .method_("add", &ivec2::add, meta::argument_("other"));

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    SUBCASE("add") {
        const meta::method add_m = ivec2_type.get_method("add");
        REQUIRE(add_m);

        CHECK(add_m.get_arity() == 1);
        CHECK(add_m.get_arguments().size() == 1);

        REQUIRE(add_m.get_argument(0));
        CHECK(add_m.get_argument(0).get_type() == meta::resolve_type<const ivec2&>());
        CHECK(add_m.get_argument(0).get_position() == 0);
        CHECK(add_m.get_argument(0).get_name() == "other");

        CHECK_FALSE(add_m.get_argument(1));
    }
}
