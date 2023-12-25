/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A { std::string a{"a"}; };
    struct B0 : virtual A { std::string b0{"b0"}; META_HPP_ENABLE_BASE_INFO(A) };
    struct B1 : virtual A { std::string b1{"b1"}; META_HPP_ENABLE_BASE_INFO(A) };

    struct C : B0, B1 { std::string c{"c"}; META_HPP_ENABLE_BASE_INFO(B0, B1) };

    struct D0 : C { std::string d0{"d0"}; META_HPP_ENABLE_BASE_INFO(C) };
    struct E0 : D0 { std::string e0{"e0"}; META_HPP_ENABLE_BASE_INFO(D0) };

    struct D1 : C { std::string d1{"d1"}; META_HPP_ENABLE_BASE_INFO(C) };
    struct E1 : D1 { std::string e1{"e1"}; META_HPP_ENABLE_BASE_INFO(D1) };
}

TEST_CASE("meta/meta_features/multiple2/_") {
    namespace meta = meta_hpp;

    //  * <- B0 <- * * <- D0 * <- E0
    // A            C
    //  * <- B1 <- * * <- D1 * <- E1
}

TEST_CASE("meta/meta_features/multiple2") {
    namespace meta = meta_hpp;

    const meta::class_type A_type = meta::resolve_type<A>();
    const meta::class_type B0_type = meta::resolve_type<B0>();
    const meta::class_type B1_type = meta::resolve_type<B1>();
    const meta::class_type C_type = meta::resolve_type<C>();
    const meta::class_type D0_type = meta::resolve_type<D0>();
    const meta::class_type D1_type = meta::resolve_type<D1>();
    const meta::class_type E0_type = meta::resolve_type<E0>();
    const meta::class_type E1_type = meta::resolve_type<E1>();

    REQUIRE(A_type);
    REQUIRE(B0_type);
    REQUIRE(B1_type);
    REQUIRE(C_type);
    REQUIRE(D0_type);
    REQUIRE(D1_type);
    REQUIRE(E0_type);
    REQUIRE(E1_type);

    SUBCASE("is_base_of") {
        CHECK(!A_type.is_base_of(A_type));
        CHECK(A_type.is_base_of(B0_type));
        CHECK(A_type.is_base_of(B1_type));
        CHECK(A_type.is_base_of(C_type));
        CHECK(A_type.is_base_of(D0_type));
        CHECK(A_type.is_base_of(D1_type));
        CHECK(A_type.is_base_of(E0_type));
        CHECK(A_type.is_base_of(E1_type));

        CHECK(!B0_type.is_base_of(A_type));
        CHECK(!B0_type.is_base_of(B0_type));
        CHECK(!B0_type.is_base_of(B1_type));
        CHECK(B0_type.is_base_of(C_type));
        CHECK(B0_type.is_base_of(D0_type));
        CHECK(B0_type.is_base_of(D1_type));
        CHECK(B0_type.is_base_of(E0_type));
        CHECK(B0_type.is_base_of(E1_type));

        CHECK(!B1_type.is_base_of(A_type));
        CHECK(!B1_type.is_base_of(B0_type));
        CHECK(!B1_type.is_base_of(B1_type));
        CHECK(B1_type.is_base_of(C_type));
        CHECK(B1_type.is_base_of(D0_type));
        CHECK(B1_type.is_base_of(D1_type));
        CHECK(B1_type.is_base_of(E0_type));
        CHECK(B1_type.is_base_of(E1_type));

        CHECK(!C_type.is_base_of(A_type));
        CHECK(!C_type.is_base_of(B0_type));
        CHECK(!C_type.is_base_of(B1_type));
        CHECK(!C_type.is_base_of(C_type));
        CHECK(C_type.is_base_of(D0_type));
        CHECK(C_type.is_base_of(D1_type));
        CHECK(C_type.is_base_of(E0_type));
        CHECK(C_type.is_base_of(E1_type));

        CHECK(!D0_type.is_base_of(A_type));
        CHECK(!D0_type.is_base_of(B0_type));
        CHECK(!D0_type.is_base_of(B1_type));
        CHECK(!D0_type.is_base_of(C_type));
        CHECK(!D0_type.is_base_of(D0_type));
        CHECK(!D0_type.is_base_of(D1_type));
        CHECK(D0_type.is_base_of(E0_type));
        CHECK(!D0_type.is_base_of(E1_type));

        CHECK(!D1_type.is_base_of(A_type));
        CHECK(!D1_type.is_base_of(B0_type));
        CHECK(!D1_type.is_base_of(B1_type));
        CHECK(!D1_type.is_base_of(C_type));
        CHECK(!D1_type.is_base_of(D0_type));
        CHECK(!D1_type.is_base_of(D1_type));
        CHECK(!D1_type.is_base_of(E0_type));
        CHECK(D1_type.is_base_of(E1_type));

        CHECK(!E0_type.is_base_of(A_type));
        CHECK(!E0_type.is_base_of(B0_type));
        CHECK(!E0_type.is_base_of(B1_type));
        CHECK(!E0_type.is_base_of(C_type));
        CHECK(!E0_type.is_base_of(D0_type));
        CHECK(!E0_type.is_base_of(D1_type));
        CHECK(!E0_type.is_base_of(E0_type));
        CHECK(!E0_type.is_base_of(E1_type));

        CHECK(!E1_type.is_base_of(A_type));
        CHECK(!E1_type.is_base_of(B0_type));
        CHECK(!E1_type.is_base_of(B1_type));
        CHECK(!E1_type.is_base_of(C_type));
        CHECK(!E1_type.is_base_of(D0_type));
        CHECK(!E1_type.is_base_of(D1_type));
        CHECK(!E1_type.is_base_of(E0_type));
        CHECK(!E1_type.is_base_of(E1_type));
    }

    SUBCASE("pointer_upcast") {
        using meta::detail::type_registry;
        using meta::detail::pointer_upcast;

        type_registry& r{type_registry::instance()};

        {
            A a;
            CHECK(pointer_upcast<A>(r, &a) == &a); CHECK(pointer_upcast<A>(r, &a)->a == "a");
            CHECK_FALSE(pointer_upcast<B0>(r, &a));
            CHECK_FALSE(pointer_upcast<B1>(r, &a));
            CHECK_FALSE(pointer_upcast<C>(r, &a));
        }

        {
            B0 b0;
            CHECK(pointer_upcast<A>(r, &b0) == &b0); CHECK(pointer_upcast<A>(r, &b0)->a == "a");
            CHECK(pointer_upcast<B0>(r, &b0) == &b0); CHECK(pointer_upcast<B0>(r, &b0)->b0 == "b0");
            CHECK_FALSE(pointer_upcast<B1>(r, &b0));
            CHECK_FALSE(pointer_upcast<C>(r, &b0));
        }

        {
            B1 b1;
            CHECK(pointer_upcast<A>(r, &b1) == &b1); CHECK(pointer_upcast<A>(r, &b1)->a == "a");
            CHECK_FALSE(pointer_upcast<B0>(r, &b1));
            CHECK(pointer_upcast<B1>(r, &b1) == &b1); CHECK(pointer_upcast<B1>(r, &b1)->b1 == "b1");
            CHECK_FALSE(pointer_upcast<C>(r, &b1));
        }

        {
            C c;
            CHECK(pointer_upcast<A>(r, &c) == &c); CHECK(pointer_upcast<A>(r, &c)->a == "a");
            CHECK(pointer_upcast<B0>(r, &c) == &c); CHECK(pointer_upcast<B0>(r, &c)->b0 == "b0");
            CHECK(pointer_upcast<B1>(r, &c) == &c); CHECK(pointer_upcast<B1>(r, &c)->b1 == "b1");
            CHECK(pointer_upcast<C>(r, &c) == &c); CHECK(pointer_upcast<C>(r, &c)->c == "c");
            CHECK_FALSE(pointer_upcast<D0>(r, &c));
            CHECK_FALSE(pointer_upcast<D1>(r, &c));
            CHECK_FALSE(pointer_upcast<E0>(r, &c));
            CHECK_FALSE(pointer_upcast<E1>(r, &c));
        }

        {
            E0 e0;
            CHECK(pointer_upcast<A>(r, &e0) == &e0); CHECK(pointer_upcast<A>(r, &e0)->a == "a");
            CHECK(pointer_upcast<B0>(r, &e0) == &e0); CHECK(pointer_upcast<B0>(r, &e0)->b0 == "b0");
            CHECK(pointer_upcast<B1>(r, &e0) == &e0); CHECK(pointer_upcast<B1>(r, &e0)->b1 == "b1");
            CHECK(pointer_upcast<C>(r, &e0) == &e0); CHECK(pointer_upcast<C>(r, &e0)->c == "c");
            CHECK(pointer_upcast<D0>(r, &e0) == &e0); CHECK(pointer_upcast<D0>(r, &e0)->d0 == "d0");
            CHECK_FALSE(pointer_upcast<D1>(r, &e0));
            CHECK(pointer_upcast<E0>(r, &e0) == &e0); CHECK(pointer_upcast<E0>(r, &e0)->e0 == "e0");
            CHECK_FALSE(pointer_upcast<E1>(r, &e0));
        }

        {
            E1 e1;
            CHECK(pointer_upcast<A>(r, &e1) == &e1); CHECK(pointer_upcast<A>(r, &e1)->a == "a");
            CHECK(pointer_upcast<B0>(r, &e1) == &e1); CHECK(pointer_upcast<B0>(r, &e1)->b0 == "b0");
            CHECK(pointer_upcast<B1>(r, &e1) == &e1); CHECK(pointer_upcast<B1>(r, &e1)->b1 == "b1");
            CHECK(pointer_upcast<C>(r, &e1) == &e1); CHECK(pointer_upcast<C>(r, &e1)->c == "c");
            CHECK_FALSE(pointer_upcast<D0>(r, &e1));
            CHECK(pointer_upcast<D1>(r, &e1) == &e1); CHECK(pointer_upcast<D1>(r, &e1)->d1 == "d1");
            CHECK_FALSE(pointer_upcast<E0>(r, &e1));
            CHECK(pointer_upcast<E1>(r, &e1) == &e1); CHECK(pointer_upcast<E1>(r, &e1)->e1 == "e1");
        }
    }
}
