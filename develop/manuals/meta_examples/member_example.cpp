/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

namespace
{
    struct ivec2 {
        int x{};
        int y{};
    };
}

TEST_CASE("meta/meta_examples/member/type") {
    namespace meta = meta_hpp;

    // 'ivec2' class type registration
    meta::class_<ivec2>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y);

    // resolves a member type by pointer
    const meta::member_type x_member_type = meta::resolve_type(&ivec2::x);

    // also, it can be resolved by static member type
    CHECK(x_member_type == meta::resolve_type<int ivec2::*>());

    // allows to know the owner class type
    CHECK(x_member_type.get_owner_type() == meta::resolve_type<ivec2>());

    // ... and the value type of the member
    CHECK(x_member_type.get_value_type() == meta::resolve_type<int>());
}

TEST_CASE("meta/meta_examples/member/usage") {
    namespace meta = meta_hpp;

    ivec2 v{42, 21};

    // resolves a class type by instance 'v'
    const meta::class_type ivec2_type = meta::resolve_type(v);

    // finds the member 'x' by name
    const meta::member ivec2_x = ivec2_type.get_member("x");

    // checks the type of the member 'x'
    CHECK(ivec2_x.get_type() == meta::resolve_type<int ivec2::*>());

    // retrieves a member value of the class instance 'v'
    const meta::uvalue ivec2_x_value = ivec2_x.get(v);

    // checks the type of the retrieved value
    CHECK(ivec2_x_value.get_type() == meta::resolve_type<int>());

    // casts the dynamic value to the typed value
    const int ivec2_x_typed_value = ivec2_x_value.as<int>();

    // here is our member typed value
    CHECK(ivec2_x_typed_value == 42);

    // also, we can change the member value, of course
    ivec2_x.set(v, 11);

    // checks the result of our manipulations
    CHECK(v.x == 11);
}
