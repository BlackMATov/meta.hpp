/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A {};
    enum class E {};
}

TEST_CASE("meta/meta_issues/random/2") {
    namespace meta = meta_hpp;

    CHECK(meta::resolve_type<A[]>() == meta::resolve_type<const A[]>());
    CHECK(meta::resolve_type<A[]>() == meta::resolve_type<volatile A[]>());
    CHECK(meta::resolve_type<A[]>() == meta::resolve_type<const volatile A[]>());

    CHECK(meta::resolve_type<A>() == meta::resolve_type<const A>());
    CHECK(meta::resolve_type<A>() == meta::resolve_type<volatile A>());
    CHECK(meta::resolve_type<A>() == meta::resolve_type<const volatile A>());

    CHECK(meta::resolve_type<E>() == meta::resolve_type<const E>());
    CHECK(meta::resolve_type<E>() == meta::resolve_type<volatile E>());
    CHECK(meta::resolve_type<E>() == meta::resolve_type<const volatile E>());

    CHECK(meta::resolve_type<int>() == meta::resolve_type<const int>());
    CHECK(meta::resolve_type<int>() == meta::resolve_type<volatile int>());
    CHECK(meta::resolve_type<int>() == meta::resolve_type<const volatile int>());

    meta::detail::type_registry& r = meta::detail::type_registry::instance();

    CHECK(r.resolve_by_type<A[]>() == r.resolve_by_type<const A[]>());
    CHECK(r.resolve_by_type<A[]>() == r.resolve_by_type<volatile A[]>());
    CHECK(r.resolve_by_type<A[]>() == r.resolve_by_type<const volatile A[]>());

    CHECK(r.resolve_by_type<A>() == r.resolve_by_type<const A>());
    CHECK(r.resolve_by_type<A>() == r.resolve_by_type<volatile A>());
    CHECK(r.resolve_by_type<A>() == r.resolve_by_type<const volatile A>());

    CHECK(r.resolve_by_type<E>() == r.resolve_by_type<const E>());
    CHECK(r.resolve_by_type<E>() == r.resolve_by_type<volatile E>());
    CHECK(r.resolve_by_type<E>() == r.resolve_by_type<const volatile E>());

    CHECK(r.resolve_by_type<int>() == r.resolve_by_type<const int>());
    CHECK(r.resolve_by_type<int>() == r.resolve_by_type<volatile int>());
    CHECK(r.resolve_by_type<int>() == r.resolve_by_type<const volatile int>());
}
