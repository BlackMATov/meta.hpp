/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_manuals.hpp"

namespace
{
    class shape {
    public:
        shape() = default;
        shape(const shape&) = default;
        virtual ~shape() = default;

        virtual int get_area() const = 0;
    };

    class rectangle : public shape {
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

TEST_CASE("meta/meta_examples/uvalue") {
    namespace meta = meta_hpp;

    // 'shape' class type registration
    meta::class_<shape>()
        .method_("get_area", &shape::get_area);

    // 'rectangle' class type registration
    meta::class_<rectangle>()
        .base_<shape>()
        .constructor_<int, int>()
        .method_("get_width", &rectangle::get_width)
        .method_("get_height", &rectangle::get_height);
}

TEST_CASE("meta/meta_examples/uvalue/usage") {
    namespace meta = meta_hpp;

    // the 'uvalue' class allows to store any copyable value inside
    // it's widely used as return value types and as containers for storing metadata

    meta::uvalue val{42};
    CHECK(val.get_type() == meta::resolve_type<int>());

    // we can get a reference to the stored data and even change it
    val.get_as<int>() = 21;
    CHECK(val.get_as<int>() == 21);

    // uvalue can be copied, assigned, and moved
    val = rectangle{10, 20};
    CHECK(val.get_type() == meta::resolve_type<rectangle>());

    // also, it supports upcasting for registered types
    CHECK(val.get_as<shape>().get_area() == 200);

    // upcasting is supported for pointers too
    rectangle rect{3, 5};
    val = &rect;
    CHECK(val.get_as<shape*>()->get_area() == 15);
    CHECK(val.get_type() == meta::resolve_type<rectangle*>());
}
