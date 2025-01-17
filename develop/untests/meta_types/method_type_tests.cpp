/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        int& at(std::size_t i) {
            switch ( i ) {
            case 0: return x;
            case 1: return y;
            default: std::abort();
            }
        }

        int length2() const noexcept {
            return x * x + y * y;
        }
    };
}

TEST_CASE("meta/meta_types/method_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::method_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("ivec2::at") {
        const meta::method_type type = meta::resolve_type(&ivec2::at);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(&ivec2::at).get_id());
        CHECK(type.get_flags() == meta::method_flags{});

        CHECK(type.get_arity() == 1);
        CHECK(type.get_owner_type() == meta::resolve_type<ivec2>());
        CHECK(type.get_return_type() == meta::resolve_type<int&>());
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<std::size_t>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<std::size_t>());
        CHECK_FALSE(type.get_argument_type(1));
    }

    SUBCASE("ivec2::length2") {
        const meta::method_type type = meta::resolve_type(&ivec2::length2);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(&ivec2::length2).get_id());
        CHECK(type.get_flags() == (meta::method_flags::is_const | meta::method_flags::is_noexcept));

        CHECK(type.get_arity() == 0);
        CHECK(type.get_owner_type() == meta::resolve_type<ivec2>());
        CHECK(type.get_return_type() == meta::resolve_type<int>());
        CHECK(type.get_argument_types() == meta::any_type_list{});

        CHECK_FALSE(type.get_argument_type(0));
    }
}
