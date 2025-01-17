/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

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

    int function_overloaded(int i0) { return i0; }
    int function_overloaded(int i0, int i1) { return i0 + i1; }

    ivec2 static_ivec2 = ivec2{1, 0};
    const ivec3 static_const_ivec3 = ivec3{1, 0};
}

TEST_CASE("meta/meta_states/scope") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::static_scope_("meta/meta_states/scope/math")
        .typedef_<color>("color")
        .typedef_<ivec2>("ivec2")
        .typedef_<ivec3>("ivec3")
        .function_("iadd2", &iadd2, meta::arguments_()("l")("r"), meta::metadata_()("desc", "iadd2"s))
        .function_("iadd3", &iadd3, meta::arguments_()("l"), meta::metadata_()("desc", "iadd3"s))
        .function_("function_overloaded", meta::select_overload<int(int)>(&function_overloaded))
        .function_("function_overloaded", meta::select_overload<int(int,int)>(&function_overloaded))
        .variable_("static_ivec2", &static_ivec2, meta::metadata_()("desc", "static_ivec2"s))
        .variable_("static_const_ivec3", &static_const_ivec3, meta::metadata_()("desc", "static_const_ivec3"s));

    const meta::scope math_scope = meta::resolve_scope("meta/meta_states/scope/math");
    REQUIRE(math_scope);
    REQUIRE(math_scope.is_valid());

    CHECK(math_scope.get_name() == "meta/meta_states/scope/math");
    CHECK(math_scope.get_variables().size() == 2);
    CHECK(math_scope.get_typedefs().size() == 3);

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
        CHECK_FALSE(math_scope.get_typedef("non-existent-class"));

        const meta::any_type ivec2_type = math_scope.get_typedef("ivec2");
        CHECK(ivec2_type == meta::resolve_type<ivec2>());

        const meta::any_type ivec3_type = math_scope.get_typedef("ivec3");
        CHECK(ivec3_type == meta::resolve_type<ivec3>());
    }

    SUBCASE("enums") {
        CHECK_FALSE(math_scope.get_typedef("non-existent-enum"));

        const meta::any_type color_type = math_scope.get_typedef("color");
        CHECK(color_type == meta::resolve_type<color>());
    }

    SUBCASE("functions") {
        CHECK_FALSE(math_scope.get_function("non-existent-function"));

        const meta::function iadd2_func = math_scope.get_function("iadd2");
        REQUIRE(iadd2_func);

        {
            CHECK(iadd2_func.get_arguments().size() == 2);

            REQUIRE(iadd2_func.get_argument(0));
            CHECK(iadd2_func.get_argument(0).get_type() == meta::resolve_type<const ivec2&>());
            CHECK(iadd2_func.get_argument(0).get_position() == 0);
            CHECK(iadd2_func.get_argument(0).get_name() == "l");

            REQUIRE(iadd2_func.get_argument(1));
            CHECK(iadd2_func.get_argument(1).get_type() == meta::resolve_type<const ivec2&>());
            CHECK(iadd2_func.get_argument(1).get_position() == 1);
            CHECK(iadd2_func.get_argument(1).get_name() == "r");

            CHECK_FALSE(iadd2_func.get_argument(2));

            REQUIRE(iadd2_func.get_metadata().contains("desc"));
            CHECK(iadd2_func.get_metadata().at("desc").as<std::string>() == "iadd2"s);
        }

        const meta::function iadd3_func = math_scope.get_function("iadd3");
        REQUIRE(iadd3_func);

        {
            CHECK(iadd3_func.get_arguments().size() == 2);

            REQUIRE(iadd3_func.get_argument(0));
            CHECK(iadd3_func.get_argument(0).get_type() == meta::resolve_type<const ivec3&>());
            CHECK(iadd3_func.get_argument(0).get_position() == 0);
            CHECK(iadd3_func.get_argument(0).get_name() == "l");

            REQUIRE(iadd3_func.get_argument(1));
            CHECK(iadd3_func.get_argument(1).get_type() == meta::resolve_type<const ivec3&>());
            CHECK(iadd3_func.get_argument(1).get_position() == 1);
            CHECK(iadd3_func.get_argument(1).get_name() == "");

            CHECK_FALSE(iadd3_func.get_argument(2));

            REQUIRE(iadd3_func.get_metadata().contains("desc"));
            CHECK(iadd3_func.get_metadata().at("desc").as<std::string>() == "iadd3"s);
        }
    }

    SUBCASE("function_with") {
        CHECK(math_scope.get_function("function_overloaded"));

        {
            CHECK_FALSE(math_scope.get_function_with<>("function_overloaded"));
            CHECK(math_scope.get_function_with<int>("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with<int, float>("function_overloaded"));

            CHECK_FALSE(math_scope.get_function_with<>("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with<float>("function_overloaded"));
            CHECK(math_scope.get_function_with<int, int>("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with<float, float>("function_overloaded"));

            CHECK_FALSE(math_scope.get_function_with<>("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with<double>("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with<double, double>("function_overloaded"));
        }

        {
            meta::number_type int_type = meta::resolve_type<int>();
            meta::number_type float_type = meta::resolve_type<float>();
            meta::number_type double_type = meta::resolve_type<double>();

            CHECK_FALSE(math_scope.get_function_with<>("function_overloaded"));
            CHECK(math_scope.get_function_with("function_overloaded", {int_type}));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", {int_type, float_type}));

            CHECK_FALSE(math_scope.get_function_with("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", {float_type}));
            CHECK(math_scope.get_function_with("function_overloaded", {int_type, int_type}));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", {float_type, float_type}));

            CHECK_FALSE(math_scope.get_function_with<>("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", {double_type}));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", {double_type, double_type}));
        }

        {
            meta::number_type int_type = meta::resolve_type<int>();
            meta::number_type float_type = meta::resolve_type<float>();
            meta::number_type double_type = meta::resolve_type<double>();

            CHECK_FALSE(math_scope.get_function_with<>("function_overloaded"));
            CHECK(math_scope.get_function_with("function_overloaded", meta::any_type_list{int_type}));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", meta::any_type_list{int_type, float_type}));

            CHECK_FALSE(math_scope.get_function_with("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", meta::any_type_list{float_type}));
            CHECK(math_scope.get_function_with("function_overloaded", meta::any_type_list{int_type, int_type}));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", meta::any_type_list{float_type, float_type}));

            CHECK_FALSE(math_scope.get_function_with<>("function_overloaded"));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", meta::any_type_list{double_type}));
            CHECK_FALSE(math_scope.get_function_with("function_overloaded", meta::any_type_list{double_type, double_type}));
        }
    }

    SUBCASE("variables") {
        CHECK_FALSE(math_scope.get_variable("non-existent-variable"));

        const meta::variable static_ivec2_var = math_scope.get_variable("static_ivec2");
        REQUIRE(static_ivec2_var);
        CHECK(static_ivec2_var.get_type().get_data_type() == meta::resolve_type<ivec2>());
        REQUIRE(static_ivec2_var.get_metadata().contains("desc"));
        CHECK(static_ivec2_var.get_metadata().at("desc").as<std::string>() == "static_ivec2"s);

        const meta::variable static_const_ivec3_var = math_scope.get_variable("static_const_ivec3");
        REQUIRE(static_const_ivec3_var);
        CHECK(static_const_ivec3_var.get_type().get_data_type() == meta::resolve_type<ivec3>());
        REQUIRE(static_const_ivec3_var.get_metadata().contains("desc"));
        CHECK(static_const_ivec3_var.get_metadata().at("desc").as<std::string>() == "static_const_ivec3"s);
    }
}

TEST_CASE("meta/meta_states/extend_scope") {
    namespace meta = meta_hpp;

    const meta::scope math_scope = meta::local_scope_("math")
        .typedef_<ivec2>("ivec2");

    CHECK(math_scope.get_typedefs().size() == 1);

    meta::extend_scope_(math_scope)
        .typedef_<ivec3>("ivec3");

    CHECK(math_scope.get_typedefs().size() == 2);
}
