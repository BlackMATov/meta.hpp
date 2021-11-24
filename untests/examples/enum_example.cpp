/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    enum class color : unsigned {
        red = 0xFF0000,
        green = 0x00FF00,
        blue = 0x0000FF,
        white = red | green | blue,
    };
}

TEST_CASE("meta/examples/enum") {
    namespace meta = meta_hpp;

    meta::enum_<color>()
        .evalue_("red", color::red)
        .evalue_("green", color::green)
        .evalue_("blue", color::blue)
        .evalue_("white", color::white);

    meta::enum_type color_type = meta::resolve_type<color>();
    REQUIRE(color_type);

    CHECK(color_type.get_underlying_type() == meta::resolve_type<unsigned>());

    CHECK(color_type.get_evalue("green").get_index().name == "green");
    CHECK(color_type.get_evalue("green").get_value() == color::green);
    CHECK_FALSE(color_type.get_evalue("yellow"));
}
