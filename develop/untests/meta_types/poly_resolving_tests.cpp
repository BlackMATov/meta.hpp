/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace {
    struct A {
        A() = default;
        A(const A&) = default;
        virtual ~A() = default;
        META_HPP_ENABLE_POLY_INFO()
    };

    struct B : A {
        META_HPP_ENABLE_POLY_INFO(A)
    };
}

TEST_CASE("meta/meta_types/poly_resolving") {
    namespace meta = meta_hpp;

    static_assert(std::is_same_v<meta::array_type, decltype(meta::resolve_type(std::declval<int[]>()))>);
    static_assert(std::is_same_v<meta::array_type, decltype(meta::resolve_type(std::declval<const int[]>()))>);

    static_assert(std::is_same_v<meta::pointer_type, decltype(meta::resolve_type(std::declval<int*>()))>);
    static_assert(std::is_same_v<meta::pointer_type, decltype(meta::resolve_type(std::declval<const int*>()))>);

    static_assert(std::is_same_v<meta::number_type, decltype(meta::resolve_type(std::declval<int>()))>);
    static_assert(std::is_same_v<meta::number_type, decltype(meta::resolve_type(std::declval<int&>()))>);
    static_assert(std::is_same_v<meta::number_type, decltype(meta::resolve_type(std::declval<int&&>()))>);
    static_assert(std::is_same_v<meta::number_type, decltype(meta::resolve_type(std::declval<const int&>()))>);
    static_assert(std::is_same_v<meta::number_type, decltype(meta::resolve_type(std::declval<const int&&>()))>);

    static_assert(std::is_same_v<meta::class_type, decltype(meta::resolve_type(std::declval<A>()))>);
    static_assert(std::is_same_v<meta::class_type, decltype(meta::resolve_type(std::declval<A&>()))>);
    static_assert(std::is_same_v<meta::class_type, decltype(meta::resolve_type(std::declval<A&&>()))>);
    static_assert(std::is_same_v<meta::class_type, decltype(meta::resolve_type(std::declval<const A&>()))>);
    static_assert(std::is_same_v<meta::class_type, decltype(meta::resolve_type(std::declval<const A&&>()))>);

    {
        int i{42};

        CHECK(meta::resolve_type(i) == meta::resolve_type<int>());
        CHECK(meta::resolve_type(std::move(i)) == meta::resolve_type<int>());
        CHECK(meta::resolve_type(std::as_const(i)) == meta::resolve_type<int>());
        CHECK(meta::resolve_type(std::move(std::as_const(i))) == meta::resolve_type<int>());
    }

    {
        A a{};

        CHECK(meta::resolve_type(a) == meta::resolve_type<A>());
        CHECK(meta::resolve_type(std::move(a)) == meta::resolve_type<A>());
        CHECK(meta::resolve_type(std::as_const(a)) == meta::resolve_type<A>());
        CHECK(meta::resolve_type(std::move(std::as_const(a))) == meta::resolve_type<A>());
    }

    {
        B b{};
        A& a{b};

        CHECK(meta::resolve_type(a) == meta::resolve_type<B>());
        CHECK(meta::resolve_type(std::move(a)) == meta::resolve_type<B>());
        CHECK(meta::resolve_type(std::as_const(a)) == meta::resolve_type<B>());
        CHECK(meta::resolve_type(std::move(std::as_const(a))) == meta::resolve_type<B>());

        CHECK(meta::resolve_type(b) == meta::resolve_type<B>());
        CHECK(meta::resolve_type(std::move(b)) == meta::resolve_type<B>());
        CHECK(meta::resolve_type(std::as_const(b)) == meta::resolve_type<B>());
        CHECK(meta::resolve_type(std::move(std::as_const(b))) == meta::resolve_type<B>());
    }
}
