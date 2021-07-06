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
                meta::field_("x", &ivec2::x),
                meta::field_("y", &ivec2::y),
                meta::method_<&ivec2::dot>("dot"),
                meta::variable_<&ivec2::zero>("zero")
            ),
            meta::class_<ivec3>("ivec3")(
                meta::field_("x", &ivec3::x),
                meta::field_("y", &ivec3::y),
                meta::field_("z", &ivec3::z),
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

    SUBCASE("resolve/class") {
        ivec2 v2;
        ivec3 v3;

        REQUIRE(registry.resolve<ivec2>());
        REQUIRE(registry.resolve(v2));
        REQUIRE(registry.resolve(std::as_const(v2)));
        REQUIRE(registry.resolve(static_cast<ivec2&&>(v2)));

        REQUIRE(registry.resolve<ivec3>());
        REQUIRE(registry.resolve(v3));
        REQUIRE(registry.resolve(std::as_const(v3)));
        REQUIRE(registry.resolve(static_cast<ivec3&&>(v3)));

        const meta::class_info v2_info = meta::class_<ivec2>("ivec2");
        const meta::class_info v3_info = meta::class_<ivec3>("ivec3");
        CHECK(v2_info.fid() != v3_info.fid());

        CHECK(v2_info.fid() == registry.resolve<ivec2>()->fid());
        CHECK(v2_info.fid() == registry.resolve(v2)->fid());
        CHECK(v2_info.fid() == registry.resolve(std::as_const(v2))->fid());
        CHECK(v2_info.fid() == registry.resolve(static_cast<ivec2&&>(v2))->fid());

        CHECK(v3_info.fid() == registry.resolve<ivec3>()->fid());
        CHECK(v3_info.fid() == registry.resolve(v3)->fid());
        CHECK(v3_info.fid() == registry.resolve(std::as_const(v3))->fid());
        CHECK(v3_info.fid() == registry.resolve(static_cast<ivec3&&>(v3))->fid());

        {
            REQUIRE(registry.get_class_by_name("vmath::ivec2"));
            REQUIRE(registry.get_class_by_name("vmath::ivec3"));

            CHECK(v2_info.fid() == registry.get_class_by_name("vmath::ivec2")->fid());
            CHECK(v3_info.fid() == registry.get_class_by_name("vmath::ivec3")->fid());
        }
    }

    SUBCASE("resolve/field") {
        REQUIRE(registry.get_field_by_name("vmath::ivec2::x"));
        REQUIRE(registry.get_field_by_name("vmath::ivec2::y"));
        REQUIRE(registry.get_field_by_name("vmath::ivec3::x"));
        REQUIRE(registry.get_field_by_name("vmath::ivec3::y"));

        CHECK("x" == registry.get_field_by_name("vmath::ivec2::x")->id());
        CHECK("y" == registry.get_field_by_name("vmath::ivec2::y")->id());
        CHECK("x" == registry.get_field_by_name("vmath::ivec3::x")->id());
        CHECK("y" == registry.get_field_by_name("vmath::ivec3::y")->id());
    }

    SUBCASE("resolve/function") {
        REQUIRE(registry.get_function_by_name("vmath::iadd2"));
        REQUIRE(registry.get_function_by_name("vmath::iadd3"));

        CHECK("iadd2" == registry.get_function_by_name("vmath::iadd2")->id());
        CHECK("iadd3" == registry.get_function_by_name("vmath::iadd3")->id());
    }

    SUBCASE("resolve/method") {
        REQUIRE(registry.get_method_by_name("vmath::ivec2::dot"));
        REQUIRE(registry.get_method_by_name("vmath::ivec3::dot"));

        CHECK("dot" == registry.get_method_by_name("vmath::ivec2::dot")->id());
        CHECK("dot" == registry.get_method_by_name("vmath::ivec3::dot")->id());
    }

    SUBCASE("resolve/variable") {
        REQUIRE(registry.get_variable_by_name("vmath::ivec2::zero"));
        REQUIRE(registry.get_variable_by_name("vmath::ivec3::zero"));

        CHECK("zero" == registry.get_variable_by_name("vmath::ivec2::zero")->id());
        CHECK("zero" == registry.get_variable_by_name("vmath::ivec3::zero")->id());
    }
}

TEST_CASE("meta/registry/merge") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    auto registry = meta::registry{};

    SUBCASE("class") {
        registry(
            meta::class_<ivec2>("ivec2")(
                meta::field_("x", &ivec2::x)
            ));

        registry(
            meta::class_<ivec2>("ivec2")(
                meta::field_("y", &ivec2::y)
            ));

        CHECK(registry.resolve<ivec2>());
        CHECK(registry.resolve<ivec2>()->get_field("x"));
        CHECK(registry.resolve<ivec2>()->get_field("y"));

        CHECK(registry.resolve(ivec2{}));
        CHECK(registry.resolve(ivec2{})->get_field("x"));
        CHECK(registry.resolve(ivec2{})->get_field("y"));

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
