/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_manuals.hpp"

namespace
{
    struct ivec2 {
        int x{}, y{};
    };

    struct ivec3 {
        int x{}, y{}, z{};
    };

    const ivec2 unit2{1, 1};
    const ivec3 unit3{1, 1, 1};

    [[maybe_unused]]
    int dot2(const ivec2& a, const ivec2& b) {
        return a.x * b.x
             + a.y * b.y;
    }

    [[maybe_unused]]
    int dot3(const ivec3& a, const ivec3& b) {
        return a.x * b.x
             + a.y * b.y
             + a.z * b.z;
    }

    [[maybe_unused]]
    bool operator==(const ivec2& a, const ivec2& b) {
        return a.x == b.x
            && a.y == b.y;
    }

    [[maybe_unused]]
    bool operator==(const ivec3& a, const ivec3& b) {
        return a.x == b.x
            && a.y == b.y
            && a.z == b.z;
    }
}

TEST_CASE("meta/meta_manuals/scopes/local") {
    namespace meta = meta_hpp;

    // creates new local scope
    // you can use it inplace or store anywhere to use it later
    const meta::scope math_scope = meta::local_scope_("math")
        .typedef_<ivec2>("ivec2")
        .typedef_<ivec3>("ivec3")
        .function_("dot2", &dot2)
        .function_("dot3", &dot3)
        .variable_("unit2", &unit2)
        .variable_("unit3", &unit3);

    // finds types by names
    const meta::class_type ivec2_type = math_scope.get_typedef("ivec2").as_class();
    const meta::class_type ivec3_type = math_scope.get_typedef("ivec3").as_class();

    // checks found types
    CHECK(ivec2_type == meta::resolve_type<ivec2>());
    CHECK(ivec3_type == meta::resolve_type<ivec3>());

    // similarly, we can find free functions by name
    const meta::function dot2_function = math_scope.get_function("dot2");
    const meta::function dot3_function = math_scope.get_function("dot3");

    // calls and checks found functions
    CHECK(dot2_function(ivec2{3,4}, ivec2{5,6}).get_as<int>() == 39);
    CHECK(dot3_function(ivec3{3,4,5}, ivec3{6,7,8}).get_as<int>() == 86);

    // and free variables, of course
    const meta::variable unit2_variable = math_scope.get_variable("unit2");
    const meta::variable unit3_variable = math_scope.get_variable("unit3");

    // checks and uses found variables with functions
    CHECK(unit2_variable.get_as<ivec2>() == ivec2{1,1});
    CHECK(unit3_variable.get_as<ivec3>() == ivec3{1,1,1});
    CHECK(dot2_function(unit2_variable.get(), unit2_variable.get()).get_as<int>() == 2);
    CHECK(dot3_function(unit3_variable.get(), unit3_variable.get()).get_as<int>() == 3);
}

TEST_CASE("meta/meta_manuals/scopes/global") {
    namespace meta = meta_hpp;

    // also, we can create global scopes
    meta::static_scope_("meta/meta_manuals/scopes/global/math")
        .function_("dot2", &dot2)
        .function_("dot3", &dot3)
        .variable_("unit2", &unit2)
        .variable_("unit3", &unit3);

    // finds our global scope by name
    const meta::scope math_scope = meta::resolve_scope("meta/meta_manuals/scopes/global/math");

    // and uses it in the same way
    const meta::function dot2_function = math_scope.get_function("dot2");
    const meta::function dot3_function = math_scope.get_function("dot3");
    const meta::variable unit2_variable = math_scope.get_variable("unit2");
    const meta::variable unit3_variable = math_scope.get_variable("unit3");

    // everything works as expected
    CHECK(dot2_function(unit2_variable.get(), unit2_variable.get()).get_as<int>() == 2);
    CHECK(dot3_function(unit3_variable.get(), unit3_variable.get()).get_as<int>() == 3);
}
