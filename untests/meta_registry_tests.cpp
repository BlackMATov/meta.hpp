/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_registry.hpp>
#include "doctest/doctest.hpp"

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        static ivec2 zero;

        int dot(ivec2 other) const {
            return x * other.x + y * other.y;
        }
    };

    struct ivec3 {
        int x{};
        int y{};
        int z{};

        static ivec3 zero;

        int dot(ivec3 other) const {
            return x * other.x + y * other.y + z * other.z;
        }
    };

    ivec2 ivec2::zero{};
    ivec3 ivec3::zero{};

    ivec2 iadd2(ivec2 l, ivec2 r) {
        return {l.x + r.x, l.y + r.y};
    }

    ivec3 iadd3(ivec3 l, ivec3 r) {
        return {l.x + r.x, l.y + r.y, l.z + r.z};
    }
}

TEST_CASE("meta/registry") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    auto registry = meta::registry{}(
        meta::namespace_("vmath")(
            meta::class_<ivec2>("ivec2")(
                meta::field_<&ivec2::x>("x"),
                meta::field_<&ivec2::y>("y"),
                meta::method_<&ivec2::dot>("dot"),
                meta::variable_<&ivec2::zero>("zero")
            ),
            meta::class_<ivec3>("ivec3")(
                meta::field_<&ivec3::x>("x"),
                meta::field_<&ivec3::y>("y"),
                meta::field_<&ivec3::z>("z"),
                meta::method_<&ivec3::dot>("dot"),
                meta::variable_<&ivec3::zero>("zero")
            ),
            meta::function_<&iadd2>("iadd2"),
            meta::function_<&iadd3>("iadd3")
        )
    );

    SUBCASE("get_xxx_by_name") {
        CHECK(registry.get_class_by_name("vmath::ivec2"));
        CHECK(registry.get_field_by_name("vmath::ivec2::x"));
        CHECK(registry.get_function_by_name("vmath::iadd2"));
        CHECK(registry.get_method_by_name("vmath::ivec2::dot"));
        CHECK(registry.get_namespace_by_name("vmath"));
        CHECK(registry.get_variable_by_name("vmath::ivec2::zero"));
    }

    SUBCASE("class_template") {
        CHECK(registry.resolve<ivec2>());

        const meta::type ivec2_type = registry.resolve<ivec2>().value();
        CHECK(ivec2_type.is_class());

        const meta::class_info ivec2_info = ivec2_type.get_class().value();
        CHECK(ivec2_info.id() == "ivec2");
    }

    SUBCASE("class_instance") {
        ivec3 v3;
        CHECK(registry.resolve(v3));

        const meta::type ivec3_type = registry.resolve(v3).value();
        CHECK(ivec3_type.is_class());

        const meta::class_info ivec3_info = ivec3_type.get_class().value();
        CHECK(ivec3_info.id() == "ivec3");
    }

    SUBCASE("field_template") {
        REQUIRE(registry.resolve<&ivec2::x>());
        REQUIRE(registry.resolve<&ivec2::y>());

        const meta::type ivec2_x_type = registry.resolve<&ivec2::x>().value();
        const meta::type ivec2_y_type = registry.resolve<&ivec2::y>().value();

        REQUIRE(ivec2_x_type.is_field());
        REQUIRE(ivec2_y_type.is_field());

        const meta::field_info ivec2_x_info = ivec2_x_type.get_field().value();
        const meta::field_info ivec2_y_info = ivec2_y_type.get_field().value();

        CHECK(ivec2_x_info.id() == "x");
        CHECK(ivec2_y_info.id() == "y");
    }

    SUBCASE("field_instance") {
        CHECK_FALSE(registry.resolve(&ivec2::x));
    }

    SUBCASE("function_template") {
        REQUIRE(registry.resolve<&iadd2>());

        const meta::type iadd2_type = registry.resolve<&iadd2>().value();
        REQUIRE(iadd2_type.is_function());

        const meta::function_info iadd2_info = iadd2_type.get_function().value();
        CHECK(iadd2_info.id() == "iadd2");
    }

    SUBCASE("function_instance") {
        CHECK_FALSE(registry.resolve(&iadd3));
    }

    SUBCASE("method_template") {
        REQUIRE(registry.resolve<&ivec2::dot>());

        const meta::type ivec2_dot_type = registry.resolve<&ivec2::dot>().value();
        REQUIRE(ivec2_dot_type.is_method());

        const meta::method_info ivec2_dot_info = ivec2_dot_type.get_method().value();
        CHECK(ivec2_dot_info.id() == "dot");
    }

    SUBCASE("method_instance") {
        CHECK_FALSE(registry.resolve(&ivec3::dot));
    }

    SUBCASE("variable_template") {
        REQUIRE(registry.resolve<&ivec2::zero>());

        const meta::type ivec2_zero_type = registry.resolve<&ivec2::zero>().value();
        REQUIRE(ivec2_zero_type.is_variable());

        const meta::variable_info ivec2_x_info = ivec2_zero_type.get_variable().value();
        CHECK(ivec2_x_info.id() == "zero");
    }

    SUBCASE("variable_instance") {
        CHECK_FALSE(registry.resolve(&ivec3::zero));
    }
}

TEST_CASE("meta/registry/merge") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    auto registry = meta::registry{};

    SUBCASE("class") {
        registry(
            meta::class_<ivec2>("ivec2")(
                meta::field_<&ivec2::x>("x")
            ));

        registry(
            meta::class_<ivec2>("ivec2")(
                meta::field_<&ivec2::y>("y")
            ));

        CHECK(registry.resolve<ivec2>());
        CHECK(registry.resolve<ivec2>()->get_class());
        CHECK(registry.resolve<ivec2>()->get_class()->get_field("x"));
        CHECK(registry.resolve<ivec2>()->get_class()->get_field("y"));

        CHECK(registry.resolve(ivec2{}));
        CHECK(registry.resolve(ivec2{})->get_class());
        CHECK(registry.resolve(ivec2{})->get_class()->get_field("x"));
        CHECK(registry.resolve(ivec2{})->get_class()->get_field("y"));

        CHECK(registry.get_class_by_name("ivec2"));
        CHECK(registry.get_class_by_name("ivec2")->get_field("x"));
        CHECK(registry.get_class_by_name("ivec2")->get_field("y"));
    }

    SUBCASE("namespace") {
        registry(
            meta::namespace_("vmath")(
                meta::class_<ivec2>("ivec2")));

        registry(
            meta::namespace_("vmath")(
                meta::class_<ivec3>("ivec3")));

        CHECK(registry.get_namespace_by_name("vmath"));
        CHECK(registry.get_namespace_by_name("vmath")->get_class("ivec2"));
        CHECK(registry.get_namespace_by_name("vmath")->get_class("ivec3"));
    }
}
