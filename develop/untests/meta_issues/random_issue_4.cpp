/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

#if !defined(META_HPP_NO_EXCEPTIONS)

namespace
{
    struct throw_on_copy {
        throw_on_copy() = default;

        [[noreturn]] throw_on_copy(const throw_on_copy&) {
            throw 42;
        }
    };

    struct throw_on_move {
        throw_on_move() = default;

        [[noreturn]] throw_on_move(throw_on_move&&) {
            throw 42;
        }

        [[noreturn]] throw_on_move(const throw_on_move&) {
            throw 42;
        }
    };
}

TEST_CASE("meta/meta_issues/random/4") {
    namespace meta = meta_hpp;

    meta::uvalue v{42};
    CHECK(v.get_type() == meta::resolve_type<int>());

    SUBCASE("1") {
        CHECK_THROWS(v = throw_on_copy{});
        CHECK(v.get_type() == meta::resolve_type<int>());
    }

    SUBCASE("2") {
        CHECK_THROWS(v = throw_on_move{});
        CHECK(v.get_type() == meta::resolve_type<int>());
    }

    SUBCASE("3") {
        meta::uvalue v2{std::in_place_type<throw_on_copy>};
        CHECK_THROWS(v = v2);
        CHECK(v.get_type() == meta::resolve_type<int>());
        CHECK_NOTHROW(v = std::move(v2));
        CHECK(v.get_type() == meta::resolve_type<throw_on_copy>());
    }

    SUBCASE("4") {
        meta::uvalue v2{std::in_place_type<throw_on_move>};
        CHECK_THROWS(v = v2);
        CHECK(v.get_type() == meta::resolve_type<int>());
        CHECK_NOTHROW(v = std::move(v2));
        CHECK(v.get_type() == meta::resolve_type<throw_on_move>());
    }
}

#endif
