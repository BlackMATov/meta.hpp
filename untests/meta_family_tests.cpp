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

TEST_CASE("meta/family") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    SUBCASE("class") {
        meta::class_info ivec2_info = meta::class_<ivec2>("ivec2");
        meta::class_info ivec3_info = meta::class_<ivec3>("ivec3");
        CHECK_FALSE(ivec2_info.fid() == ivec3_info.fid());
    }

    SUBCASE("field") {
        meta::field_info x_info = meta::field_<&ivec2::x>("x");
        meta::field_info y_info = meta::field_<&ivec2::y>("y");
        CHECK_FALSE(x_info.fid() == y_info.fid());
    }

    SUBCASE("function") {
        meta::function_info iadd2_info = meta::function_<&iadd2>("iadd2");
        meta::function_info iadd3_info = meta::function_<&iadd3>("iadd3");
        CHECK_FALSE(iadd2_info.fid() == iadd3_info.fid());
    }

    SUBCASE("method") {
        meta::method_info dot2_info = meta::method_<&ivec2::dot>("dot");
        meta::method_info dot3_info = meta::method_<&ivec3::dot>("dot");
        CHECK_FALSE(dot2_info.fid() == dot3_info.fid());
    }

    SUBCASE("variable") {
        meta::variable_info zero2_info = meta::variable_<&ivec2::zero>("zero");
        meta::variable_info zero3_info = meta::variable_<&ivec3::zero>("zero");
        CHECK_FALSE(zero2_info.fid() == zero3_info.fid());
    }
}
