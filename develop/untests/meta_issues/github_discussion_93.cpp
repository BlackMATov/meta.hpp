/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    enum class align {
        left,
        right,
        center,
    };

    enum class color {
        red,
        green,
        blue,
    };
}

TEST_CASE("meta/meta_discussion/93") {
    namespace meta = meta_hpp;

    meta::enum_<align>()
        .evalue_("left", align::left)
        .evalue_("right", align::right)
        .evalue_("center", align::center);

    const meta::enum_type align_type = meta::resolve_type<align>();

    {
        CHECK(align_type.name_to_evalue("left").get_name() == "left");
        CHECK(align_type.name_to_evalue("right").get_name() == "right");
        CHECK(align_type.name_to_evalue("center").get_name() == "center");
    }

    {
        align v = align::right;

        CHECK(align_type.value_to_evalue(v).get_name() == "right");
        CHECK(align_type.value_to_evalue(std::as_const(v)).get_name() == "right");
        CHECK(align_type.value_to_evalue(std::move(std::as_const(v))).get_name() == "right");
        CHECK(align_type.value_to_evalue(std::move(v)).get_name() == "right");

        v = align::center;

        CHECK(align_type.value_to_evalue(v).get_name() == "center");
        CHECK(align_type.value_to_evalue(std::as_const(v)).get_name() == "center");
        CHECK(align_type.value_to_evalue(std::move(std::as_const(v))).get_name() == "center");
        CHECK(align_type.value_to_evalue(std::move(v)).get_name() == "center");
    }

    {
        meta::uvalue v = align::right;

        CHECK(align_type.value_to_evalue(v).get_name() == "right");
        CHECK(align_type.value_to_evalue(std::as_const(v)).get_name() == "right");
        CHECK(align_type.value_to_evalue(std::move(std::as_const(v))).get_name() == "right");
        CHECK(align_type.value_to_evalue(std::move(v)).get_name() == "right");

        v = align::center;

        CHECK(align_type.value_to_evalue(v).get_name() == "center");
        CHECK(align_type.value_to_evalue(std::as_const(v)).get_name() == "center");
        CHECK(align_type.value_to_evalue(std::move(std::as_const(v))).get_name() == "center");
        CHECK(align_type.value_to_evalue(std::move(v)).get_name() == "center");
    }

    {
        CHECK_FALSE(align_type.name_to_evalue(""));
        CHECK_FALSE(align_type.name_to_evalue("middle"));

        CHECK_FALSE(align_type.value_to_evalue(color::red));
        CHECK_FALSE(align_type.value_to_evalue(meta::uvalue{color::red}));
    }
}
