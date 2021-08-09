/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_infos_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

namespace
{
    class shape {
    public:
        virtual ~shape() {}
        virtual int area() const = 0;
    };

    class rectangle : public shape {
    public:
        rectangle(const rectangle& other) {
            width_ = other.width_;
            height_ = other.height_;
        }

        rectangle& operator=(const rectangle& other) {
            if ( this != &other ) {
                return *this;
            }
            width_ = other.width_;
            height_ = other.height_;
            return *this;
        }

        rectangle(int width, int height)
        : width_{width}
        , height_{height} {}

        int area() const override {
            return width_ * height_;
        }

        int width() const noexcept {
            return width_;
        }

        int height() const noexcept {
            return height_;
        }
    private:
        int width_{};
        int height_{};
    };
}

TEST_CASE("features/infos/class") {
    registry db;

    db(
        class_<shape>{"shape"}(
            method_{"area", &shape::area}
        ),
        class_<rectangle>{"rectangle"}(
            base_<shape>{},
            ctor_<int, int>{},
            method_{"width", &rectangle::width},
            method_{"height", &rectangle::height}
        )
    );

    SUBCASE("base") {
        REQUIRE(db.get_class_by_path("shape"));
        const class_info shape_info = db.get_class_by_path("shape");
        CHECK(shape_info.name() == "shape");
        CHECK(shape_info.type().id() == type_db::get<shape>().id());

        CHECK_FALSE(shape_info.is_derived_from<shape>());
        CHECK_FALSE(shape_info.is_derived_from<rectangle>());
    }

    SUBCASE("derived") {
        REQUIRE(db.get_class_by_path("rectangle"));
        const class_info rectangle_info = db.get_class_by_path("rectangle");
        CHECK(rectangle_info.name() == "rectangle");
        CHECK(rectangle_info.type().id() == type_db::get<rectangle>().id());

        CHECK(rectangle_info.is_derived_from<shape>());
        CHECK_FALSE(rectangle_info.is_derived_from<rectangle>());
    }
}
