/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

namespace
{
    class shape {
        META_HPP_ENABLE_POLY_INFO()
    public:
        shape() = default;
        shape(const shape&) = default;
        virtual ~shape() = default;

        virtual int get_area() const = 0;
    };

    class rectangle : public shape {
        META_HPP_ENABLE_POLY_INFO(shape)
    public:
        explicit rectangle(int width, int height)
        : width_{width}
        , height_{height} {}

        int get_width() const {
            return width_;
        }

        int get_height() const {
            return height_;
        }

        int get_area() const override {
            return width_ * height_;
        }
    private:
        int width_{};
        int height_{};
    };
}

TEST_CASE("meta/meta_manuals/class/type") {
    namespace meta = meta_hpp;

    // 'shape' class type registration
    meta::class_<shape>()
        .method_("get_area", &shape::get_area);

    // 'rectangle' class type registration
    meta::class_<rectangle>()
        .constructor_<int, int>()
        .method_("get_width", &rectangle::get_width)
        .method_("get_height", &rectangle::get_height);

    // resolves a class type by static class type
    const meta::class_type rectangle_type = meta::resolve_type<rectangle>();

    // prints all class methods
    fmt::print("* rectangle\n");
    for ( const meta::method& method : rectangle_type.get_methods() ) {
        fmt::print("  + {}/{}\n",
            method.get_name(),
            method.get_type().get_arity());
    }

    // Output:
    // * rectangle
    //   + get_height/0
    //   + get_width/0
}

TEST_CASE("meta/meta_manuals/class/usage") {
    namespace meta = meta_hpp;

    // resolves a class type by static class type
    const meta::class_type rectangle_type = meta::resolve_type<rectangle>();

    // finds the method 'get_area' by name
    const meta::method rectangle_area = rectangle_type.get_method("get_area");

    // creates a rectangle instance by the registered constructor(int, int)
    const meta::uvalue rectangle_v = rectangle_type.create(10, 20);

    // calls the method with the dynamic rectangle instance 'rectangle_v'
    CHECK(rectangle_area.invoke(rectangle_v).as<int>() == 200);
}
