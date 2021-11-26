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

    struct derived_ivec2 : ivec2 {
        [[maybe_unused]] derived_ivec2() = default;
        [[maybe_unused]] explicit derived_ivec2(int v): ivec2{v, v} {}
        [[maybe_unused]] derived_ivec2(int x, int y): ivec2{x, y} {}
    };
}

TEST_CASE("features/meta_utilities/inst") {
    namespace meta = meta_hpp;

    SUBCASE("ref") {
        {
            derived_ivec2 v{1,2};
            derived_ivec2& vr = v;
            meta::detail::inst a{vr};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::ref);
        }
        {
            meta::value v{derived_ivec2{1,2}};
            meta::value& vr = v;
            meta::detail::inst a{vr};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::ref);

            CHECK(a.can_cast_to<derived_ivec2>());
            CHECK(a.can_cast_to<derived_ivec2&>());
            CHECK(a.can_cast_to<const derived_ivec2&>());
            CHECK_FALSE(a.can_cast_to<derived_ivec2&&>());
            CHECK_FALSE(a.can_cast_to<const derived_ivec2&&>());
        }
    }

    SUBCASE("cref") {
        {
            const derived_ivec2 v{1,2};
            const derived_ivec2& vr = v;
            meta::detail::inst a{vr};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::cref);
        }
        {
            const meta::value v{derived_ivec2{1,2}};
            const meta::value& vr = v;
            meta::detail::inst a{vr};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::cref);

            CHECK_FALSE(a.can_cast_to<derived_ivec2>());
            CHECK_FALSE(a.can_cast_to<derived_ivec2&>());
            CHECK(a.can_cast_to<const derived_ivec2&>());
            CHECK_FALSE(a.can_cast_to<derived_ivec2&&>());
            CHECK_FALSE(a.can_cast_to<const derived_ivec2&&>());
        }
    }

    SUBCASE("rref") {
        {
            derived_ivec2 v{1,2};
            meta::detail::inst a{std::move(v)};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::rref);
        }
        {
            meta::value v{derived_ivec2{1,2}};
            meta::detail::inst a{std::move(v)};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::rref);

            CHECK(a.can_cast_to<derived_ivec2>());
            CHECK_FALSE(a.can_cast_to<derived_ivec2&>());
            CHECK_FALSE(a.can_cast_to<const derived_ivec2&>());
            CHECK(a.can_cast_to<derived_ivec2&&>());
            CHECK(a.can_cast_to<const derived_ivec2&&>());
        }
    }

    SUBCASE("crref") {
        {
            const derived_ivec2 v{1,2};
            meta::detail::inst a{std::move(v)};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::crref);
        }
        {
            const meta::value v{derived_ivec2{1,2}};
            meta::detail::inst a{std::move(v)};

            CHECK(a.get_raw_type() == meta::resolve_type<derived_ivec2>());
            CHECK(a.get_ref_type() == meta::detail::inst::ref_types::crref);

            CHECK_FALSE(a.can_cast_to<derived_ivec2>());
            CHECK_FALSE(a.can_cast_to<derived_ivec2&>());
            CHECK_FALSE(a.can_cast_to<const derived_ivec2&>());
            CHECK_FALSE(a.can_cast_to<derived_ivec2&&>());
            CHECK(a.can_cast_to<const derived_ivec2&&>());
        }
    }
}
