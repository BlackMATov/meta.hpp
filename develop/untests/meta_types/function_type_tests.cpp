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
    };

    void arg_copy(ivec2) {}
    void arg_ref_noexcept(ivec2&) noexcept {}
    void arg_cref_noexcept(const ivec2&) noexcept {}

    void arg_bounded_arr(ivec2[2]) {}
    void arg_unbounded_arr(ivec2[]) {}
    void arg_bounded_const_arr(const ivec2[2]) {}
    void arg_unbounded_const_arr(const ivec2[]) {}
}

TEST_CASE("meta/meta_types/function_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::function_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("arg_copy") {
        const meta::function_type type = meta::resolve_type(arg_copy);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(arg_copy).get_id());
        CHECK(type.get_flags() == meta::function_flags{});

        CHECK(type.get_arity() == 1);
        CHECK(type.get_return_type() == meta::resolve_type<void>());
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<ivec2>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<ivec2>());
        CHECK_FALSE(type.get_argument_type(1));
    }

    SUBCASE("arg_ref_noexcept") {
        const meta::function_type type = meta::resolve_type(arg_ref_noexcept);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(arg_ref_noexcept).get_id());
        CHECK(type.get_flags() == meta::function_flags::is_noexcept);
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<ivec2&>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<ivec2&>());
        CHECK_FALSE(type.get_argument_type(1));
    }

    SUBCASE("arg_cref_noexcept") {
        const meta::function_type type = meta::resolve_type(arg_cref_noexcept);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(arg_cref_noexcept).get_id());
        CHECK(type.get_flags() == meta::function_flags::is_noexcept);
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<const ivec2&>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<const ivec2&>());
        CHECK_FALSE(type.get_argument_type(1));
    }

    SUBCASE("arg_bounded_arr") {
        const meta::function_type type = meta::resolve_type(arg_bounded_arr);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(arg_bounded_arr).get_id());
        CHECK(type.get_flags() == meta::function_flags{});
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<ivec2*>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<ivec2*>());
        CHECK_FALSE(type.get_argument_type(1));
    }

    SUBCASE("arg_unbounded_arr") {
        const meta::function_type type = meta::resolve_type(arg_unbounded_arr);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(arg_unbounded_arr).get_id());
        CHECK(type.get_flags() == meta::function_flags{});
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<ivec2*>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<ivec2*>());
        CHECK_FALSE(type.get_argument_type(1));
    }

    SUBCASE("arg_bounded_const_arr") {
        const meta::function_type type = meta::resolve_type(arg_bounded_const_arr);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(arg_bounded_const_arr).get_id());
        CHECK(type.get_flags() == meta::function_flags{});
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<const ivec2*>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<const ivec2*>());
        CHECK_FALSE(type.get_argument_type(1));
    }

    SUBCASE("arg_unbounded_const_arr") {
        const meta::function_type type = meta::resolve_type(arg_unbounded_const_arr);
        REQUIRE(type);

        CHECK(type.get_id() == meta::resolve_type(arg_unbounded_const_arr).get_id());
        CHECK(type.get_flags() == meta::function_flags{});
        CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<const ivec2*>()});

        CHECK(type.get_argument_type(0) == meta::resolve_type<const ivec2*>());
        CHECK_FALSE(type.get_argument_type(1));
    }
}
