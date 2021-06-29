/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta.hpp>
#include "doctest/doctest.hpp"

namespace
{
    class ivec2 {
    public:
        int x{};
        int y{};

        ivec2() = default;
        explicit ivec2(int v): x{v}, y{v} {}
        ivec2(int x, int y): x{x}, y{y} {}

        int dot(ivec2 other) const {
            return x * other.x + y * other.y;
        }

        int length2() const {
            return dot(*this);
        }
    };

    class ivec3 {
    public:
        int x{};
        int y{};
        int z{};

        ivec3() = default;
        explicit ivec3(int v): x{v}, y{v}, z{v} {}
        ivec3(int x, int y, int z): x{x}, y{y}, z{z} {}

        int dot(ivec3 other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        int length2() const {
            return dot(*this);
        }
    };

    ivec2 add(ivec2 l, ivec2 r) {
        return {l.x + r.x, l.y + r.y};
    }

    ivec3 add(ivec3 l, ivec3 r) {
        return {l.x + r.x, l.y + r.y, l.z + r.z};
    }
}

TEST_CASE("meta/examples/ivec2") {
    SUBCASE("ctors") {
        CHECK(ivec2{}.x == 0);
        CHECK(ivec2{}.y == 0);

        CHECK(ivec2{1}.x == 1);
        CHECK(ivec2{1}.y == 1);

        CHECK(ivec2{2,3}.x == 2);
        CHECK(ivec2{2,3}.y == 3);
    }
}

TEST_CASE("meta/examples/ivec3") {
    SUBCASE("ctors") {
        CHECK(ivec3{}.x == 0);
        CHECK(ivec3{}.y == 0);
        CHECK(ivec3{}.z == 0);

        CHECK(ivec3{1}.x == 1);
        CHECK(ivec3{1}.y == 1);
        CHECK(ivec3{1}.z == 1);

        CHECK(ivec3{2,3,4}.x == 2);
        CHECK(ivec3{2,3,4}.y == 3);
        CHECK(ivec3{2,3,4}.z == 4);
    }
}

TEST_CASE("meta/examples/simple") {
    namespace meta = meta_hpp;

    auto registry = meta::registry{}(
        meta::namespace_("vmath")(
            meta::class_<ivec2>("ivec2")(
                meta::field_<&ivec2::x>("x"),
                meta::field_<&ivec2::y>("y"),
                meta::method_<&ivec2::dot>("dot"),
                meta::method_<&ivec2::length2>("length2")
            ),
            meta::class_<ivec3>("ivec3")(
                meta::field_<&ivec3::x>("x"),
                meta::field_<&ivec3::y>("y"),
                meta::field_<&ivec3::z>("z"),
                meta::method_<&ivec3::dot>("dot"),
                meta::method_<&ivec3::length2>("length2")
            ),
            meta::function_<meta::select<ivec2(ivec2,ivec2)>(&add)>("iadd2"),
            meta::function_<meta::select<ivec3(ivec3,ivec3)>(&add)>("iadd3")
        )
    );

    meta::namespace_info vmath_info = registry.get_namespace("vmath").value();

    meta::class_info ivec2_info = vmath_info.get_class("ivec2").value();
    meta::field_info ivec2_x_info = ivec2_info.get_field("x").value();
    meta::field_info ivec2_y_info = ivec2_info.get_field("y").value();

    meta::method_info ivec2_dot_info = ivec2_info.get_method("dot").value();
    meta::method_info ivec2_length2_info = ivec2_info.get_method("length2").value();

    meta::function_info iadd2_info = vmath_info.get_function("iadd2").value();
    meta::function_info iadd3_info = vmath_info.get_function("iadd3").value();

    {
        ivec2 v2{1,2};
        CHECK(ivec2_x_info.get(&v2).cast<int>() == 1);
        CHECK(ivec2_y_info.get(&v2).cast<int>() == 2);
        CHECK(ivec2_dot_info.invoke(&v2, v2).cast<int>() == 5);
        CHECK(ivec2_length2_info.invoke(&v2).cast<int>() == 5);
    }

    {
        ivec2 v = iadd2_info.invoke(ivec2{1,2}, ivec2{3,4}).cast<ivec2>();
        CHECK(v.x == 4);
        CHECK(v.y == 6);
    }

    {
        ivec3 v = iadd3_info.invoke(ivec3{1,2,3}, ivec3{3,4,5}).cast<ivec3>();
        CHECK(v.x == 4);
        CHECK(v.y == 6);
        CHECK(v.z == 8);
    }
}
