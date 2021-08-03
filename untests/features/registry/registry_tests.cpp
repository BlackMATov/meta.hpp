/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_registry_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        [[maybe_unused]] ivec2() = default;
        [[maybe_unused]] explicit ivec2(int v): x{v}, y{v} {}
        [[maybe_unused]] ivec2(int x, int y): x{x}, y{y} {}
    };
}

TEST_CASE("features/registry/registry") {
    auto db = registry{}(
        namespace_{"vmath"}(data_{"info", "vmath namespace"},
            class_<ivec2>{"ivec2"}(data_{"info", "ivec2 class"},
                ctor_<>{},
                ctor_<int>{},
                ctor_<int, int>{},
                member_{"x", &ivec2::x}(data_{"info", "x-coord"}),
                member_{"y", &ivec2::y}(data_{"info", "y-coord"})
            )
        )
    );

    {
        const namespace_info vmath_info = db.get_namespace_by_path("vmath");
        REQUIRE(vmath_info);
        CHECK(vmath_info.name() == "vmath");
        CHECK(vmath_info.get_data_by_name("info"));

        const class_info ivec2_info = vmath_info.get_class_by_name("ivec2");
        REQUIRE(ivec2_info);
        CHECK(ivec2_info.name() == "ivec2");
        CHECK(ivec2_info.get_data_by_name("info"));

        const member_info ivec2_x_info = ivec2_info.get_member_by_name("x");
        REQUIRE(ivec2_x_info);
        CHECK(ivec2_x_info.name() == "x");
        CHECK(ivec2_x_info.get_data_by_name("info"));
    }

    {
        const class_info ivec2_info = db.get_class_by_path("vmath::ivec2");
        REQUIRE(ivec2_info);
        CHECK(ivec2_info.name() == "ivec2");

        const member_info ivec2_x_info = db.get_member_by_path("vmath::ivec2::x");
        REQUIRE(ivec2_x_info);
        CHECK(ivec2_x_info.name() == "x");

        const data_info ivec2_x_data_info = db.get_data_by_path("vmath::ivec2::x::info");
        REQUIRE(ivec2_x_data_info);
        CHECK(ivec2_x_data_info.name() == "info");
    }
}
