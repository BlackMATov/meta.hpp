/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

namespace
{
    enum class align {
        left,
        right,
        center,
    };
}

TEST_CASE("meta/meta_examples/enum/type") {
    namespace meta = meta_hpp;

    // 'align' enumeration type registration
    meta::enum_<align>()
        .evalue_("left", align::left)
        .evalue_("right", align::right)
        .evalue_("center", align::center);

    // resolves a enumeration type by enumerator
    const meta::enum_type align_type = meta::resolve_type(align::left);

    // also, it can be resolved by static enumeration type
    CHECK(align_type == meta::resolve_type<align>());

    // allows to know the underlying type
    CHECK(align_type.get_underlying_type() == meta::resolve_type<int>());

    // prints all enumerators
    fmt::print("* align\n");
    for ( const meta::evalue& evalue : align_type.get_evalues() ) {
        fmt::print("  - {}/{}\n",
            evalue.get_name(),
            evalue.get_underlying_value().as<int>());
    }

    // Output:
    // * align
    //   - center/2
    //   - left/0
    //   - right/1
}

TEST_CASE("meta/meta_examples/enum/usage") {
    namespace meta = meta_hpp;

    const align e = align::center;

    // resolves a enumeration type by enumerator instance 'e'
    const meta::enum_type align_type = meta::resolve_type(e);

    // converts the enumerator to its name
    CHECK(align_type.value_to_evalue(e).get_name() == "center");

    // ... and back again
    CHECK(align_type.name_to_evalue("center").get_value().as<align>() == e);
}
