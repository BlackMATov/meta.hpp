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

    SUBCASE("class_template") {
        CHECK(registry.resolve<ivec2>());

        const meta::type ivec2_type = registry.resolve<ivec2>().value();
        CHECK(ivec2_type.is_class());

        const meta::class_info& ivec2_info = ivec2_type.get_class_info();
        CHECK(ivec2_info.id() == "ivec2");
    }

    SUBCASE("class_instance") {
        ivec3 v3;
        CHECK(registry.resolve(v3));

        const meta::type ivec3_type = registry.resolve(v3).value();
        CHECK(ivec3_type.is_class());

        const meta::class_info& ivec3_info = ivec3_type.get_class_info();
        CHECK(ivec3_info.id() == "ivec3");
    }

    SUBCASE("field_template") {
        CHECK(registry.resolve<&ivec2::x>());

        const meta::type ivec2_x_type = registry.resolve<&ivec2::x>().value();
        CHECK(ivec2_x_type.is_field());

        const meta::field_info& ivec2_x_info = ivec2_x_type.get_field_info();
        CHECK(ivec2_x_info.id() == "x");
    }

    SUBCASE("field_instance") {
        CHECK(registry.resolve(&ivec3::x));

        const meta::type ivec3_x_type = registry.resolve<&ivec3::x>().value();
        CHECK(ivec3_x_type.is_field());

        const meta::field_info& ivec3_x_info = ivec3_x_type.get_field_info();
        CHECK(ivec3_x_info.id() == "x");
    }

    SUBCASE("function_template") {
        CHECK(registry.resolve<&iadd2>());

        const meta::type iadd2_type = registry.resolve<&iadd2>().value();
        CHECK(iadd2_type.is_function());

        const meta::function_info& iadd2_info = iadd2_type.get_function_info();
        CHECK(iadd2_info.id() == "iadd2");
    }

    SUBCASE("function_instance") {
        CHECK(registry.resolve(&iadd3));

        const meta::type iadd3_type = registry.resolve<&iadd3>().value();
        CHECK(iadd3_type.is_function());

        const meta::function_info& iadd3_info = iadd3_type.get_function_info();
        CHECK(iadd3_info.id() == "iadd3");
    }

    SUBCASE("method_template") {
        CHECK(registry.resolve<&ivec2::dot>());

        const meta::type ivec2_dot_type = registry.resolve<&ivec2::dot>().value();
        CHECK(ivec2_dot_type.is_method());

        const meta::method_info& ivec2_dot_info = ivec2_dot_type.get_method_info();
        CHECK(ivec2_dot_info.id() == "dot");
    }

    SUBCASE("method_instance") {
        CHECK(registry.resolve(&ivec3::dot));

        const meta::type ivec3_dot_type = registry.resolve<&ivec3::dot>().value();
        CHECK(ivec3_dot_type.is_method());

        const meta::method_info& ivec3_dot_info = ivec3_dot_type.get_method_info();
        CHECK(ivec3_dot_info.id() == "dot");
    }

    SUBCASE("variable_template") {
        CHECK(registry.resolve<&ivec2::zero>());

        const meta::type ivec2_zero_type = registry.resolve<&ivec2::zero>().value();
        CHECK(ivec2_zero_type.is_variable());

        const meta::variable_info& ivec2_x_info = ivec2_zero_type.get_variable_info();
        CHECK(ivec2_x_info.id() == "zero");
    }

    SUBCASE("variable_instance") {
        CHECK(registry.resolve(&ivec3::zero));

        const meta::type ivec3_zero_type = registry.resolve<&ivec3::zero>().value();
        CHECK(ivec3_zero_type.is_variable());

        const meta::variable_info& ivec3_x_info = ivec3_zero_type.get_variable_info();
        CHECK(ivec3_x_info.id() == "zero");
    }
}
