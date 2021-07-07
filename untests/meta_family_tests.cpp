/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_class.hpp>
#include <meta.hpp/meta_field.hpp>
#include <meta.hpp/meta_function.hpp>
#include <meta.hpp/meta_method.hpp>
#include <meta.hpp/meta_variable.hpp>

#include "doctest/doctest.hpp"

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        static ivec2 zero;
        static ivec2 unit;

        ivec2& add(const ivec2 other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        ivec2& sub(const ivec2 other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }
    };

    struct ivec3 {
        int x{};
        int y{};
        int z{};

        static ivec3 zero;
        static ivec3 unit;

        ivec3& add(const ivec3 other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        ivec3& sub(const ivec3 other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
    };

    ivec2 ivec2::zero{0, 0};
    ivec2 ivec2::unit{1, 1};

    ivec3 ivec3::zero{0, 0, 0};
    ivec3 ivec3::unit{1, 1, 1};

    ivec2 iadd2(ivec2 l, ivec2 r) {
        return {l.x + r.x, l.y + r.y};
    }

    ivec2 isub2(ivec2 l, ivec2 r) {
        return {l.x - r.x, l.y - r.y};
    }

    ivec3 iadd3(ivec3 l, ivec3 r) {
        return {l.x + r.x, l.y + r.y, l.z + r.z};
    }

    ivec3 isub3(ivec3 l, ivec3 r) {
        return {l.x - r.x, l.y - r.y, l.z - r.z};
    }
}

TEST_CASE("meta/family") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    SUBCASE("class") {
        meta::class_info ivec2_info = meta::class_<ivec2>("ivec2");
        meta::class_info ivec3_info = meta::class_<ivec3>("ivec3");
        CHECK(ivec2_info.family() != ivec3_info.family());
    }

    SUBCASE("field") {
        meta::field_info ivec2_x_info = meta::field_("x", &ivec2::x);
        meta::field_info ivec2_y_info = meta::field_("y", &ivec2::y);

        meta::field_info ivec3_x_info = meta::field_("x", &ivec3::x);
        meta::field_info ivec3_y_info = meta::field_("y", &ivec3::y);

        CHECK(ivec2_x_info.family() == ivec2_y_info.family());

        CHECK(ivec2_x_info.family() != ivec3_x_info.family());
        CHECK(ivec2_x_info.family() != ivec3_y_info.family());
    }

    SUBCASE("function") {
        meta::function_info iadd2_info = meta::function_("iadd2", &iadd2);
        meta::function_info isub2_info = meta::function_("isub2", &isub2);

        meta::function_info iadd3_info = meta::function_("iadd3", &iadd3);
        meta::function_info isub3_info = meta::function_("isub3", &isub3);

        CHECK(iadd2_info.family() == isub2_info.family());
        CHECK(iadd3_info.family() == isub3_info.family());

        CHECK(iadd2_info.family() != iadd3_info.family());
        CHECK(iadd2_info.family() != isub3_info.family());
    }

    SUBCASE("method") {
        meta::method_info add2_info = meta::method_("add", &ivec2::add);
        meta::method_info sub2_info = meta::method_("sub", &ivec2::sub);

        meta::method_info add3_info = meta::method_("add", &ivec3::add);
        meta::method_info sub3_info = meta::method_("sub", &ivec3::sub);

        CHECK(add2_info.family() == sub2_info.family());
        CHECK(add3_info.family() == sub3_info.family());

        CHECK(add2_info.family() != add3_info.family());
        CHECK(add2_info.family() != sub3_info.family());
    }

    SUBCASE("variable") {
        meta::variable_info zero2_info = meta::variable_("zero", &ivec2::zero);
        meta::variable_info unit2_info = meta::variable_("unit", &ivec2::unit);

        meta::variable_info zero3_info = meta::variable_("zero", &ivec3::zero);
        meta::variable_info unit3_info = meta::variable_("unit", &ivec3::unit);

        CHECK(zero2_info.family() == unit2_info.family());
        CHECK(zero3_info.family() == unit3_info.family());

        CHECK(zero2_info.family() != zero3_info.family());
        CHECK(zero2_info.family() != unit3_info.family());
    }
}
