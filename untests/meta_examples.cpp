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
    using namespace meta_hpp;

    namespace_info vmath_info = namespace_("vmath")(
        class_<ivec2>("ivec2")(
            field_("x", &ivec2::x)(
                data_("tooltip", "x-coordinate field")
            ),
            field_("y", &ivec2::y)(
                data_("tooltip", "y-coordinate field")
            ),
            method_("dot", &ivec2::dot),
            method_("length2", &ivec2::length2)
        ),
        class_<ivec3>("ivec3")(
            field_("x", &ivec3::x)(
                data_("tooltip", "x-coordinate field")
            ),
            field_("y", &ivec3::y)(
                data_("tooltip", "y-coordinate field")
            ),
            field_("z", &ivec3::z)(
                data_("tooltip", "z-coordinate field")
            ),
            method_("dot", &ivec3::dot),
            method_("length2", &ivec3::length2)
        ),
        function_("iadd2", select<ivec2(ivec2,ivec2)>(&add)),
        function_("iadd3", select<ivec3(ivec3,ivec3)>(&add))
    );

    class_info ivec2_info = vmath_info.get_class("ivec2").value();

    field_info ivec2_x_info = ivec2_info.get_field("x").value();
    field_info ivec2_y_info = ivec2_info.get_field("y").value();

    data_info ivec2_x_tooltip_info = ivec2_x_info.get_data("tooltip").value();
    data_info ivec2_y_tooltip_info = ivec2_y_info.get_data("tooltip").value();

    method_info ivec2_dot_info = ivec2_info.get_method("dot").value();
    method_info ivec2_length2_info = ivec2_info.get_method("length2").value();

    function_info iadd2_info = vmath_info.get_function("iadd2").value();
    function_info iadd3_info = vmath_info.get_function("iadd3").value();

    {
        ivec2 v2{1,2};
        CHECK(ivec2_x_info.get(v2).cast<int>() == 1);
        CHECK(ivec2_y_info.get(v2).cast<int>() == 2);
        CHECK(ivec2_dot_info.invoke(v2, v2)->cast<int>() == 5);
        CHECK(ivec2_length2_info.invoke(v2)->cast<int>() == 5);
    }

    {
        ivec2 v = iadd2_info.invoke(ivec2{1,2}, ivec2{3,4})->cast<ivec2>();
        CHECK(v.x == 4);
        CHECK(v.y == 6);
    }

    {
        ivec3 v = iadd3_info.invoke(ivec3{1,2,3}, ivec3{3,4,5})->cast<ivec3>();
        CHECK(v.x == 4);
        CHECK(v.y == 6);
        CHECK(v.z == 8);
    }
}

TEST_CASE("meta/examples/advanced") {
    using namespace meta_hpp;

    auto db = registry{}(
        class_<ivec2>("ivec2")(
            field_("x", &ivec2::x),
            field_("y", &ivec2::y),
            method_("dot", &ivec2::dot),
            method_("length2", &ivec2::length2)
        ),
        class_<ivec3>("ivec3")(
            field_("x", &ivec3::x),
            field_("y", &ivec3::y),
            field_("z", &ivec3::z),
            method_("dot", &ivec3::dot),
            method_("length2", &ivec3::length2)
        )
    );

    ivec3 v3{1,2,3};

    type v3_type = db.resolve(v3).value();
    class_info v3_info = v3_type.get_class().value();

    std::cout << "class " << v3_info.id() << std::endl;

    v3_info.each_field([&v3](const field_info& info){
        std::cout
            << "  - " << info.id()
            << ": " << info.get(v3).to_int() << std::endl;
    });

    v3_info.each_method([](const method_info& info){
        std::cout
            << "  + " << info.id() << "()" << std::endl;
    });
}
