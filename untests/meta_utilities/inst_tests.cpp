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

        [[maybe_unused]] ivec2() = default;
        [[maybe_unused]] explicit ivec2(int v): x{v}, y{v} {}
        [[maybe_unused]] ivec2(int x, int y): x{x}, y{y} {}

        [[maybe_unused]] ivec2(ivec2&& other) noexcept {
            x = other.x;
            y = other.y;
            other.x = 0;
            other.y = 0;
        }

        [[maybe_unused]] ivec2(const ivec2& other) noexcept {
            x = other.x;
            y = other.y;
        }

        ivec2& operator=(ivec2&&) = delete;
        ivec2& operator=(const ivec2&) = delete;
    };
}

TEST_CASE("features/meta_utilities/inst") {
    namespace meta = meta_hpp;

    SUBCASE("ref") {
        ivec2 v{1,2};
        ivec2& vr = v;
        meta::detail::inst a{vr};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::inst::ref_types::ref);
    }

    SUBCASE("cref") {
        const ivec2 v{1,2};
        const ivec2& vr = v;
        meta::detail::inst a{vr};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::inst::ref_types::cref);
    }

    SUBCASE("rref") {
        ivec2 v{1,2};
        meta::detail::inst a{std::move(v)};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::inst::ref_types::rref);
    }

    SUBCASE("crref") {
        const ivec2 v{1,2};
        meta::detail::inst a{std::move(v)};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::inst::ref_types::crref);
    }
}
