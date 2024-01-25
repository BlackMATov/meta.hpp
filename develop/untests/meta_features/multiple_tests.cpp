/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A {
        A() = default;

        A(A&&) = default;
        A(const A&) = default;

        A& operator=(A&&) = delete;
        A& operator=(const A&) = delete;

        virtual ~A() = default;
    };

    struct B : A { META_HPP_ENABLE_BASE_INFO(A) };
    struct C : A { META_HPP_ENABLE_BASE_INFO(A) };
    struct D : B, C { META_HPP_ENABLE_BASE_INFO(B, C) };
    struct E : D { META_HPP_ENABLE_BASE_INFO(D) };
    struct F {};
}

TEST_CASE("meta/meta_features/multiple/_") {
    namespace meta = meta_hpp;

    // A * <- B <- *
    //              D * <- E
    // A * <- C <- *
}

TEST_CASE("meta/meta_features/multiple") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::uinst;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    const meta::class_type A_type = meta::resolve_type<A>();
    const meta::class_type B_type = meta::resolve_type<B>();
    const meta::class_type C_type = meta::resolve_type<C>();
    const meta::class_type D_type = meta::resolve_type<D>();
    const meta::class_type E_type = meta::resolve_type<E>();
    const meta::class_type F_type = meta::resolve_type<F>();

    REQUIRE(A_type);
    REQUIRE(B_type);
    REQUIRE(C_type);
    REQUIRE(D_type);
    REQUIRE(E_type);
    REQUIRE(F_type);

    SUBCASE("is_base_of") {
        CHECK(!A_type.is_base_of(A_type));
        CHECK(A_type.is_base_of(B_type));
        CHECK(A_type.is_base_of(C_type));
        CHECK(A_type.is_base_of(D_type));
        CHECK(A_type.is_base_of(E_type));
        CHECK(!A_type.is_base_of(F_type));

        CHECK(!B_type.is_base_of(A_type));
        CHECK(!B_type.is_base_of(B_type));
        CHECK(!B_type.is_base_of(C_type));
        CHECK(B_type.is_base_of(D_type));
        CHECK(B_type.is_base_of(E_type));
        CHECK(!B_type.is_base_of(F_type));

        CHECK(!C_type.is_base_of(A_type));
        CHECK(!C_type.is_base_of(B_type));
        CHECK(!C_type.is_base_of(C_type));
        CHECK(C_type.is_base_of(D_type));
        CHECK(C_type.is_base_of(E_type));
        CHECK(!C_type.is_base_of(F_type));

        CHECK(!D_type.is_base_of(A_type));
        CHECK(!D_type.is_base_of(B_type));
        CHECK(!D_type.is_base_of(C_type));
        CHECK(!D_type.is_base_of(D_type));
        CHECK(D_type.is_base_of(E_type));
        CHECK(!D_type.is_base_of(F_type));

        CHECK(!E_type.is_base_of(A_type));
        CHECK(!E_type.is_base_of(B_type));
        CHECK(!E_type.is_base_of(C_type));
        CHECK(!E_type.is_base_of(D_type));
        CHECK(!E_type.is_base_of(E_type));
        CHECK(!E_type.is_base_of(F_type));

        CHECK(!F_type.is_base_of(A_type));
        CHECK(!F_type.is_base_of(B_type));
        CHECK(!F_type.is_base_of(C_type));
        CHECK(!F_type.is_base_of(D_type));
        CHECK(!F_type.is_base_of(E_type));
        CHECK(!F_type.is_base_of(F_type));
    }

    SUBCASE("is_derived_from") {
        CHECK(!A_type.is_derived_from(A_type));
        CHECK(!A_type.is_derived_from(B_type));
        CHECK(!A_type.is_derived_from(C_type));
        CHECK(!A_type.is_derived_from(D_type));
        CHECK(!A_type.is_derived_from(E_type));
        CHECK(!A_type.is_derived_from(F_type));

        CHECK(B_type.is_derived_from(A_type));
        CHECK(!B_type.is_derived_from(B_type));
        CHECK(!B_type.is_derived_from(C_type));
        CHECK(!B_type.is_derived_from(D_type));
        CHECK(!B_type.is_derived_from(E_type));
        CHECK(!B_type.is_derived_from(F_type));

        CHECK(C_type.is_derived_from(A_type));
        CHECK(!C_type.is_derived_from(B_type));
        CHECK(!C_type.is_derived_from(C_type));
        CHECK(!C_type.is_derived_from(D_type));
        CHECK(!C_type.is_derived_from(E_type));
        CHECK(!C_type.is_derived_from(F_type));

        CHECK(D_type.is_derived_from(A_type));
        CHECK(D_type.is_derived_from(B_type));
        CHECK(D_type.is_derived_from(C_type));
        CHECK(!D_type.is_derived_from(D_type));
        CHECK(!D_type.is_derived_from(E_type));
        CHECK(!D_type.is_derived_from(F_type));

        CHECK(E_type.is_derived_from(A_type));
        CHECK(E_type.is_derived_from(B_type));
        CHECK(E_type.is_derived_from(C_type));
        CHECK(E_type.is_derived_from(D_type));
        CHECK(!E_type.is_derived_from(E_type));
        CHECK(!E_type.is_derived_from(F_type));

        CHECK(!F_type.is_derived_from(A_type));
        CHECK(!F_type.is_derived_from(B_type));
        CHECK(!F_type.is_derived_from(C_type));
        CHECK(!F_type.is_derived_from(D_type));
        CHECK(!F_type.is_derived_from(F_type));
    }

    SUBCASE("pointer_upcast") {
        using meta::detail::pointer_upcast;
        {
            A a;
            CHECK(pointer_upcast<A>(r, &a) == &a);
            CHECK_FALSE(pointer_upcast<B>(r, &a));
            CHECK_FALSE(pointer_upcast<C>(r, &a));
            CHECK_FALSE(pointer_upcast<D>(r, &a));
            CHECK_FALSE(pointer_upcast<E>(r, &a));
            CHECK_FALSE(pointer_upcast<F>(r, &a));
        }
        {
            const B b;
            CHECK(pointer_upcast<A>(r, &b) == &b);
            CHECK(pointer_upcast<B>(r, &b) == &b);
            CHECK_FALSE(pointer_upcast<C>(r, &b));
            CHECK_FALSE(pointer_upcast<D>(r, &b));
            CHECK_FALSE(pointer_upcast<E>(r, &b));
            CHECK_FALSE(pointer_upcast<F>(r, &b));
        }
        {
            C c;
            CHECK(pointer_upcast<A>(r, &c) == &c);
            CHECK_FALSE(pointer_upcast<B>(r, &c));
            CHECK(pointer_upcast<C>(r, &c) == &c);
            CHECK_FALSE(pointer_upcast<D>(r, &c));
            CHECK_FALSE(pointer_upcast<E>(r, &c));
            CHECK_FALSE(pointer_upcast<F>(r, &c));
        }
        {
            const D d;
            CHECK_FALSE(pointer_upcast<A>(r,&d));
            CHECK(pointer_upcast<A>(r, pointer_upcast<B>(r,&d)) == static_cast<const A*>(static_cast<const B*>(&d)));
            CHECK(pointer_upcast<A>(r, pointer_upcast<C>(r,&d)) == static_cast<const A*>(static_cast<const C*>(&d)));
            CHECK(pointer_upcast<B>(r, &d) == &d);
            CHECK(pointer_upcast<C>(r, &d) == &d);
            CHECK(pointer_upcast<D>(r, &d) == &d);
            CHECK_FALSE(pointer_upcast<E>(r, &d));
            CHECK_FALSE(pointer_upcast<F>(r, &d));
        }
        {
            const E e;
            CHECK_FALSE(pointer_upcast<A>(r,&e));
            CHECK(pointer_upcast<A>(r, pointer_upcast<B>(r,&e)) == static_cast<const A*>(static_cast<const B*>(&e)));
            CHECK(pointer_upcast<A>(r, pointer_upcast<C>(r,&e)) == static_cast<const A*>(static_cast<const C*>(&e)));
            CHECK(pointer_upcast<B>(r, &e) == &e);
            CHECK(pointer_upcast<C>(r, &e) == &e);
            CHECK(pointer_upcast<D>(r, &e) == &e);
            CHECK(pointer_upcast<E>(r, &e) == &e);
            CHECK_FALSE(pointer_upcast<F>(r, &e));
        }
        {
            F f;
            CHECK_FALSE(pointer_upcast<A>(r, &f));
            CHECK_FALSE(pointer_upcast<B>(r, &f));
            CHECK_FALSE(pointer_upcast<C>(r, &f));
            CHECK_FALSE(pointer_upcast<D>(r, &f));
            CHECK_FALSE(pointer_upcast<E>(r, &f));
            CHECK(pointer_upcast<F>(r, &f) == &f);
        }
    }
}
