/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        ivec2() = default;
        ivec2(int v) : x{v}, y{v} {}
        ivec2(int x, int y) : x{x}, y{y} {}

        int dot(const ivec2& other) const {
            return x * other.x + y * other.y;
        }

        int length2() const {
            return dot(*this);
        }

        friend bool operator==(const ivec2& l, const ivec2& r) {
            return l.x == r.x && l.y == r.y;
        }
    };
}

TEST_CASE("meta/examples/complex") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .ctor_<>()
        .ctor_<int>()
        .ctor_<int, int>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y)
        .method_("dot", &ivec2::dot)
        .method_("length2", &ivec2::length2);

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();

    const meta::member ivec2_x = ivec2_type.get_member("x");
    const meta::member ivec2_y = ivec2_type.get_member("y");

    const meta::method ivec2_dot = ivec2_type.get_method("dot");
    const meta::method ivec2_length2 = ivec2_type.get_method("length2");

    {
        const ivec2 v = ivec2_type()->cast<ivec2>();
        CHECK(v == ivec2{});

        CHECK(ivec2_x(v) == 0);
        CHECK(ivec2_y(v) == 0);
    }

    {
        const ivec2 v = ivec2_type(3)->cast<ivec2>();
        CHECK(v == ivec2{3});

        CHECK(ivec2_x(v) == 3);
        CHECK(ivec2_y(v) == 3);
    }

    {
        const meta::value v = ivec2_type(1, 2).value();
        CHECK(v == ivec2{1, 2});

        CHECK(ivec2_x(v) == 1);
        CHECK(ivec2_y(v) == 2);
    }

    {
        meta::value v = ivec2_type(1, 2).value();

        ivec2_x(v, 10);
        ivec2_y(v, 20);

        CHECK(ivec2_x(v) == 10);
        CHECK(ivec2_y(v) == 20);
    }

    {
        const meta::value v0 = ivec2_type(1, 2).value();
        const meta::value v1 = ivec2_type(3, 4).value();

        CHECK(ivec2_dot(v0, v1) == 1 * 3 + 2 * 4);
    }

    {
        const meta::value v = ivec2_type(3, 4).value();

        CHECK(ivec2_length2(v) == 3 * 3 + 4 * 4);
    }
}
