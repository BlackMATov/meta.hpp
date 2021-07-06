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
    };

    struct ivec3 {
        int x{};
        int y{};
        int z{};
    };
}

TEST_CASE("meta/family") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    SUBCASE("class") {
        meta::class_info ivec2_info = meta::class_<ivec2>("ivec2");
        meta::class_info ivec3_info = meta::class_<ivec3>("ivec3");
        CHECK_FALSE(ivec2_info.fid() == ivec3_info.fid());
    }
}
