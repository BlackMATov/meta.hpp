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

        ivec2& add(const ivec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        friend bool operator==(const ivec2& l, const ivec2& r) {
            return l.x == r.x && l.y == r.y;
        }
    };
}

TEST_CASE("meta/meta_manuals/method/type") {
    namespace meta = meta_hpp;

    // 'ivec2' class type registration
    meta::class_<ivec2>()
        .method_("add", &ivec2::add);

    // resolves a method type by pointer
    const meta::method_type add_method_type = meta::resolve_type(&ivec2::add);

    // also, it can be resolved by static method type
    CHECK(add_method_type == meta::resolve_type<ivec2&(ivec2::*)(const ivec2&)>());

    // allows to know the owner class type
    CHECK(add_method_type.get_owner_type() == meta::resolve_type<ivec2>());

    // checks a return value type
    CHECK(add_method_type.get_return_type() == meta::resolve_type<ivec2&>());

    // allows to know an argument count
    CHECK(add_method_type.get_arity() == 1);

    // ... and their types
    CHECK(add_method_type.get_argument_type(0) == meta::resolve_type<const ivec2&>());
}

TEST_CASE("meta/meta_manuals/method/usage") {
    namespace meta = meta_hpp;

    ivec2 v{20, 10};

    // resolves a class type by instance 'v'
    const meta::class_type ivec2_type = meta::resolve_type(v);

    // finds the method 'add' by name
    const meta::method ivec2_add = ivec2_type.get_method("add");

    // checks the type of the method 'add'
    CHECK(ivec2_add.get_type() == meta::resolve_type<ivec2&(ivec2::*)(const ivec2&)>());

    // checks the ability to call the method with specific arguments
    CHECK(ivec2_add.is_invocable_with(v, ivec2{22, 11}));
    CHECK(ivec2_add.is_invocable_with<ivec2&, ivec2>());

    // calls the method and retrieves a returned value
    const meta::uvalue ivec2_add_result_value = ivec2_add.invoke(v, ivec2{22, 11});

    // checks the type and value of the result
    CHECK(ivec2_add_result_value.get_type() == meta::resolve_type<ivec2>());
    CHECK(ivec2_add_result_value.as<ivec2>() == ivec2{42, 21});

    // checks the result of our manipulations
    CHECK(v == ivec2{42, 21});
}
