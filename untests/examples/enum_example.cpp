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

    {
        meta::number_type color_underlying_type = color_type.get_underlying_type();

        CHECK(color_underlying_type.get_size() == 4);
        CHECK(color_underlying_type.get_flags().has(meta::number_flags::is_unsigned));
    }

    {
        meta::evalue green_evalue = color_type.get_evalue("green");

        CHECK(green_evalue.get_name() == "green");
        CHECK(green_evalue.get_type() == color_type);

        CHECK(green_evalue.get_value() == color::green);
    }

    {
        CHECK(color_type.value_to_name(color::blue) == "blue");
        CHECK(color_type.name_to_value("white") == color::white);
    }
}
