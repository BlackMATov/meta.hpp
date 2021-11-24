/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct ivec2 final {
        int x{};
        int y{};

        int dot(const ivec2& other) const noexcept {
            return x * other.x + y * other.y;
        }

        int length2() const noexcept {
            return dot(*this);
        }
    };
}

TEST_CASE("meta/examples/method") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y)
        .method_("dot", &ivec2::dot)
        .method_("length2", &ivec2::length2);

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    CHECK(ivec2_type.get_methods().size() == 2);

    const meta::method ivec2_dot = ivec2_type.get_method("dot");
    REQUIRE(ivec2_dot);

    CHECK(ivec2_dot.get_index().name == "dot");
    CHECK(ivec2_dot.get_type() == meta::resolve_type<decltype(&ivec2::dot)>());

    CHECK(ivec2_dot.get_type().get_arity() == 1);
    CHECK(ivec2_dot.get_type().get_return_type() == meta::resolve_type<int>());
    CHECK(ivec2_dot.get_type().get_argument_type(0) == meta::resolve_type<const ivec2&>());
}
