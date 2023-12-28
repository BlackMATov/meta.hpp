/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct base {};
    struct derived : base {};

    struct poly_base {
        virtual ~poly_base() = default;
        META_HPP_ENABLE_POLY_INFO()
    };

    struct poly_derived : poly_base {
        META_HPP_ENABLE_POLY_INFO(poly_base)
    };
}

TEST_CASE("meta/meta_types/poly_resolving2") {
    namespace meta = meta_hpp;

    SUBCASE("simple-type") {
        int v{};
        CHECK(meta::resolve_type(v) == meta::resolve_type<int>());
    }

    SUBCASE("not-polymorphic-class") {
        derived d;
        base& b = d;
        CHECK(meta::resolve_type(b) == meta::resolve_type<base>());
        CHECK(meta::resolve_type(d) == meta::resolve_type<derived>());
    }

    SUBCASE("polymorphic-class") {
        poly_derived d;
        poly_base& b = d;
        CHECK(meta::resolve_type(b) == meta::resolve_type<poly_derived>());
        CHECK(meta::resolve_type(d) == meta::resolve_type<poly_derived>());
    }
}
