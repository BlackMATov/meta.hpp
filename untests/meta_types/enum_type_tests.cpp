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

TEST_CASE("meta/meta_types/enum_type") {
    namespace meta = meta_hpp;

    meta::enum_<color>()
        .evalue_("red", color::red)
        .evalue_("green", color::green)
        .evalue_("blue", color::blue)
        .evalue_("white", color::white);

    SUBCASE("color") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        CHECK(color_type.get_id() == meta::resolve_type(color{}).get_id());
        CHECK(color_type.get_underlying_type() == meta::resolve_type<unsigned>());

        CHECK(color_type.get_evalues().size() == 4);
    }

    SUBCASE("const color") {
        const meta::enum_type color_type = meta::resolve_type<const color>();
        REQUIRE(color_type);

        CHECK(color_type.get_id() == meta::resolve_type(color{}).get_id());
        CHECK(color_type.get_underlying_type() == meta::resolve_type<unsigned>());

        CHECK(color_type.get_evalues().size() == 4);
    }

    SUBCASE("get_evalue") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        {
            const meta::evalue green_value = color_type.get_evalue("green");
            REQUIRE(green_value);
            CHECK(green_value.get_value() == color::green);
        }

        {
            const meta::evalue yellow_value = color_type.get_evalue("yellow");
            CHECK_FALSE(yellow_value);
        }
    }

    SUBCASE("value_to_name") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        {
            REQUIRE(color_type.value_to_name(color::red));
            CHECK(color_type.value_to_name(color::red) == "red");
        }

        {

            REQUIRE(color_type.value_to_name(meta::value{color::blue}));
            CHECK(color_type.value_to_name(color::blue) == "blue");
        }

        {
            REQUIRE_FALSE(color_type.value_to_name(100500));
            REQUIRE_FALSE(color_type.value_to_name(color{100500}));
        }
    }

    SUBCASE("name_to_value") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        {
            REQUIRE(color_type.name_to_value("blue"));
            CHECK(color_type.name_to_value("blue") == color::blue);
        }

        {
            REQUIRE_FALSE(color_type.name_to_value("yellow"));
        }
    }
}
