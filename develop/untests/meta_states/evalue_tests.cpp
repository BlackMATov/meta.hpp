/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    enum class color : unsigned {
        red = 0xFF0000,
        green = 0x00FF00,
        blue = 0x0000FF,
        white = red | green | blue,
    };

    enum another_color {};
}

TEST_CASE("meta/meta_states/evalue") {
    namespace meta = meta_hpp;

    meta::enum_<color>()
        .evalue_("red", color::red)
        .evalue_("green", color::green)
        .evalue_("blue", color::blue)
        .evalue_("white", color::white);

    const meta::enum_type color_type = meta::resolve_type<color>();
    REQUIRE(color_type);

    SUBCASE("") {
        const meta::evalue evalue;
        CHECK_FALSE(evalue);
        CHECK_FALSE(evalue.is_valid());
        CHECK(evalue == color_type.get_evalue("non-existent-evalue"));
    }

    SUBCASE("operators") {
        const meta::evalue blue_e = color_type.get_evalue("blue");
        const meta::evalue white_e = color_type.get_evalue("white");
        CHECK(blue_e == blue_e);
        CHECK(blue_e != white_e);
        CHECK((blue_e < white_e || white_e < blue_e));
    }

    SUBCASE("green") {
        const meta::evalue evalue = color_type.get_evalue("green");
        REQUIRE(evalue);

        CHECK(evalue.get_index().get_type() == evalue.get_type());
        CHECK(evalue.get_index().get_name() == "green");

        CHECK(evalue.get_type() == meta::resolve_type<color>());
        CHECK(evalue.get_name() == "green");

        CHECK(evalue.get_value().as<color>() == color::green);
        CHECK(evalue.get_value().get_type() == color_type);

        CHECK(evalue.get_underlying_value().as<unsigned>() == meta::detail::to_underlying(color::green));
        CHECK(evalue.get_underlying_value().get_type() == color_type.get_underlying_type());
    }
}
