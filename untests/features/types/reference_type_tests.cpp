/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_types_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

namespace
{
    struct ivec2 {
        int x{};
        int y{};
    };
}

TEST_CASE("features/types/reference") {
    SUBCASE("ivec2&") {
        REQUIRE(type_db::get<ivec2&>());
        REQUIRE(type_db::get<ivec2&>().is<reference_type>());

        const reference_type rt = type_db::get<ivec2&>().as<reference_type>();

        CHECK(rt.data_type() == type_db::get<ivec2>());

        CHECK(rt.flags() == (reference_flags::is_lvalue));

        CHECK(rt.is_lvalue());
        CHECK_FALSE(rt.is_rvalue());
    }

    SUBCASE("const ivec2&&") {
        REQUIRE(type_db::get<const ivec2&&>());
        REQUIRE(type_db::get<const ivec2&&>().is<reference_type>());

        const reference_type rt = type_db::get<const ivec2&&>().as<reference_type>();

        CHECK(rt.data_type() == type_db::get<const ivec2>());

        CHECK(rt.flags() == (reference_flags::is_rvalue));

        CHECK_FALSE(rt.is_lvalue());
        CHECK(rt.is_rvalue());
    }
}
