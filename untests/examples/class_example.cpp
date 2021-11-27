/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    class shape {
    public:
        shape() = default;
        shape(const shape&) = default;
        shape& operator=(const shape&) = default;

        virtual ~shape() {}
        virtual double area() const = 0;
    };

    class circle : public shape {
        double radius_;
    public:
        circle(const circle&) = default;
        circle& operator=(const circle&) = default;

        explicit circle(double radius) : radius_{radius} {}
        double area() const override { return 3.1415926536 * radius_ * radius_; }

        double radius() const { return radius_; }
    };

    class rectangle : public shape {
        double width_;
        double height_;
    public:
        rectangle(const rectangle&) = default;
        rectangle& operator=(const rectangle&) = default;

        explicit rectangle(double width, double height) : width_{width}, height_{height} {}
        double area() const override { return width_ * height_; }

        double width() const { return width_; }
        double height() const { return height_; }
    };
}

TEST_CASE("meta/examples/class") {
    namespace meta = meta_hpp;

    meta::class_<shape>()
        .method_("area", &shape::area);

    meta::class_<circle>()
        .base_<shape>()
        .ctor_<double>()
        .method_("radius", &circle::radius);

    meta::class_<rectangle>()
        .base_<shape>()
        .ctor_<double, double>()
        .method_("width", &rectangle::width)
        .method_("height", &rectangle::height);

    const meta::scope geometry = meta::local_scope_("geometry")
        .class_<shape>("shape")
        .class_<circle>("circle")
        .class_<rectangle>("rectangle");

    const meta::class_type shape_type = geometry.get_class("shape");
    const meta::method area_method = shape_type.get_method("area");

    {
        const meta::class_type circle_type = geometry.get_class("circle");

        const meta::method radius_method = circle_type.get_method("radius");

        const meta::value circle_v = circle_type.create(5.0).value();

        const meta::value circle_area_v = area_method(circle_v).value();
        const meta::value circle_radius_v = radius_method(circle_v).value();

        CHECK(circle_area_v.cast<double>() == doctest::Approx(78.5).epsilon(0.1));
        CHECK(circle_radius_v == 5.0);
    }

    {
        const meta::class_type rectangle_type = geometry.get_class("rectangle");

        const meta::method width_method = rectangle_type.get_method("width");
        const meta::method height_method = rectangle_type.get_method("height");

        const meta::value rectangle_v = rectangle_type(10.0, 20.0).value();

        const meta::value rectangle_area_v = area_method(rectangle_v).value();
        const meta::value rectangle_width_v = width_method(rectangle_v).value();
        const meta::value rectangle_height_v = height_method(rectangle_v).value();

        CHECK(rectangle_area_v.cast<double>() == doctest::Approx(200.0).epsilon(0.1));
        CHECK(rectangle_width_v == 10.0);
        CHECK(rectangle_height_v == 20.0);
    }
}
