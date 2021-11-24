/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct ivec2 final {
        int x{};
        int y{};
    };
}

TEST_CASE("meta/examples/member") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y);

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    CHECK(ivec2_type.get_members().size() == 2);

    const meta::member ivec2_x = ivec2_type.get_member("x");
    REQUIRE(ivec2_x);

    CHECK(ivec2_x.get_index().name == "x");
    CHECK(ivec2_x.get_type() == meta::resolve_type<decltype(&ivec2::x)>());

    CHECK(ivec2_x.get_type().get_owner_type() == meta::resolve_type<ivec2>());
    CHECK(ivec2_x.get_type().get_value_type() == meta::resolve_type<int>());
}
