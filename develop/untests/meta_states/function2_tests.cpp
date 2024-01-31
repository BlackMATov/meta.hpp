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

        static ivec2 iadd(const ivec2& l, const ivec2& r) noexcept {
            return {l.x + r.x, l.y + r.y};
        }

        static ivec2 isub(const ivec2& l, const ivec2& r) noexcept {
            return {l.x - r.x, l.y - r.y};
        }
    };
}

TEST_CASE("meta/meta_states/function2") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .function_("iadd", &ivec2::iadd, meta::arguments_()("l"))
        .function_("isub", &ivec2::isub, meta::arguments_()("l")("r"));

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    SUBCASE("iadd2") {
        const meta::function func = ivec2_type.get_function("iadd");
        REQUIRE(func);

        CHECK(func.get_arity() == 2);
        CHECK(func.get_arguments().size() == 2);

        REQUIRE(func.get_argument(0));
        CHECK(func.get_argument(0).get_type() == meta::resolve_type<const ivec2&>());
        CHECK(func.get_argument(0).get_position() == 0);
        CHECK(func.get_argument(0).get_name() == "l");

        REQUIRE(func.get_argument(1));
        CHECK(func.get_argument(1).get_type() == meta::resolve_type<const ivec2&>());
        CHECK(func.get_argument(1).get_position() == 1);
        CHECK(func.get_argument(1).get_name() == "");

        CHECK_FALSE(func.get_argument(2));
    }

    SUBCASE("isub2") {
        const meta::function func = ivec2_type.get_function("isub");
        REQUIRE(func);

        CHECK(func.get_arity() == 2);
        REQUIRE(func.get_arguments().size() == 2);

        REQUIRE(func.get_argument(0));
        CHECK(func.get_argument(0).get_type() == meta::resolve_type<const ivec2&>());
        CHECK(func.get_argument(0).get_position() == 0);
        CHECK(func.get_argument(0).get_name() == "l");

        REQUIRE(func.get_argument(1));
        CHECK(func.get_argument(1).get_type() == meta::resolve_type<const ivec2&>());
        CHECK(func.get_argument(1).get_position() == 1);
        CHECK(func.get_argument(1).get_name() == "r");

        CHECK_FALSE(func.get_argument(2));
    }
}
