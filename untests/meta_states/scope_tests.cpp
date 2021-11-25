/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    enum class color {
        red,
        green,
        blue,
    };

    struct ivec2 {
        int x{};
        int y{};
    };

    struct ivec3 {
        int x{};
        int y{};
        int z{};
    };

    ivec2 iadd2(const ivec2& l, const ivec2& r) noexcept {
        return {l.x + r.x, l.y + r.y};
    }

    ivec3 iadd3(const ivec3& l, const ivec3& r) noexcept {
        return {l.x + r.x, l.y + r.y, l.z + r.z};
    }

    ivec2 global_ivec2 = ivec2{1, 0};
    const ivec3 global_const_ivec3 = ivec3{1, 0};
}

TEST_CASE("meta/meta_states/scope") {
    namespace meta = meta_hpp;

    meta::scope_("meta/meta_states/scope/math")
        .enum_<color>("color")
        .class_<ivec2>("ivec2")
        .class_<ivec3>("ivec3")
        .function_("iadd2", &iadd2)
        .function_("iadd3", &iadd3)
        .variable_("global_ivec2", &global_ivec2)
        .variable_("global_const_ivec3", &global_const_ivec3);

    const meta::scope math_scope = meta::resolve_scope("meta/meta_states/scope/math");
    REQUIRE(math_scope);
    REQUIRE(math_scope.is_valid());

    CHECK(math_scope.get_name() == "meta/meta_states/scope/math");
    CHECK(math_scope.get_classes().size() == 2);
    CHECK(math_scope.get_enums().size() == 1);
    CHECK(math_scope.get_functions().size() == 2);
    CHECK(math_scope.get_variables().size() == 2);

    SUBCASE("") {
        const meta::scope scope;
        CHECK_FALSE(scope);
        CHECK_FALSE(scope.is_valid());
    }

    SUBCASE("operators") {
        const meta::scope math1_s = meta::resolve_scope("meta/meta_states/scope/math1");
        const meta::scope math2_s = meta::resolve_scope("meta/meta_states/scope/math2");
        CHECK(math1_s == math1_s);
        CHECK(math1_s != math2_s);
        CHECK((math1_s < math2_s || math2_s < math1_s));
    }

    SUBCASE("classes") {
        CHECK_FALSE(math_scope.get_class("non-existent-class"));

        const meta::class_type ivec2_type = math_scope.get_class("ivec2");
        REQUIRE(ivec2_type);

        const meta::class_type ivec3_type = math_scope.get_class("ivec3");
        REQUIRE(ivec3_type);
    }

    SUBCASE("enums") {
        CHECK_FALSE(math_scope.get_enum("non-existent-enum"));

        const meta::enum_type color_type = math_scope.get_enum("color");
        REQUIRE(color_type);
    }

    SUBCASE("functions") {
        CHECK_FALSE(math_scope.get_function("non-existent-function"));

        const meta::function iadd2_func = math_scope.get_function("iadd2");
        REQUIRE(iadd2_func);

        const meta::function iadd3_func = math_scope.get_function("iadd3");
        REQUIRE(iadd3_func);
    }

    SUBCASE("variables") {
        CHECK_FALSE(math_scope.get_variable("non-existent-variable"));

        const meta::variable global_ivec2_var = math_scope.get_variable("global_ivec2");
        REQUIRE(global_ivec2_var);
        CHECK(global_ivec2_var.get_type().get_data_type() == meta::resolve_type<ivec2>());

        const meta::variable global_const_ivec3_var = math_scope.get_variable("global_const_ivec3");
        REQUIRE(global_const_ivec3_var);
        CHECK(global_const_ivec3_var.get_type().get_data_type() == meta::resolve_type<ivec3>());
    }
}