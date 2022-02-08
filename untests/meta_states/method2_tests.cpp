/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

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
        .method_("add", &ivec2::add, {"other"});

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    SUBCASE("add") {
        const meta::method add_m = ivec2_type.get_method("add");
        REQUIRE(add_m);

        CHECK(add_m.get_parameters().size() == 1);

        REQUIRE(add_m.get_parameter(0));
        CHECK(add_m.get_parameter(0).get_type() == meta::resolve_type<const ivec2&>());
        CHECK(add_m.get_parameter(0).get_position() == 0);
        CHECK(add_m.get_parameter(0).get_name() == "other");

        CHECK_FALSE(add_m.get_parameter(1));
    }
}
