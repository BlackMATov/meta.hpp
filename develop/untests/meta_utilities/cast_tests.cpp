/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A1 {
        A1() = default;
        A1(const A1&) = default;
        virtual ~A1() = default;
        char a{'a'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct B1 : A1 {
        B1() = default;
        B1(const B1&) = default;
        char b{'b'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct C1 : B1 {
        C1() = default;
        C1(const C1&) = default;
        char c{'c'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    // A1 <- B1 <- C1

    struct A2 {
        A2() = default;
        A2(const A2&) = default;
        virtual ~A2() = default;
        char a{'a'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct B2 : virtual A2 {
        B2() = default;
        B2(const B2&) = default;
        char b{'b'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct C2 : virtual A2 {
        C2() = default;
        C2(const C2&) = default;
        char c{'c'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };


    struct D2 {
        D2() = default;
        D2(const D2&) = default;
        virtual ~D2() = default;
        char d{'d'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct E2 : D2 {
        E2() = default;
        E2(const E2&) = default;
        char e{'e'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct F2 : B2, C2, E2 {
        F2() = default;
        F2(const F2&) = default;
        char f{'f'};
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };


    // A2 <= B2
    //          <-
    // A2 <= C2    F2
    //          <-
    // D2 <- E2

    namespace meta = meta_hpp;

    template < meta::detail::class_kind From, meta::detail::class_kind To, typename Value >
    void check_casts_impl(Value& value) {
        using from_void_cv = meta::detail::add_cv_t<From, void>;
        from_void_cv* from_void_ptr = dynamic_cast<from_void_cv*>(std::addressof(value));
        REQUIRE(from_void_ptr == meta::ucast<from_void_cv*>(std::addressof(value)));

        From* from_ptr = dynamic_cast<From*>(std::addressof(value));
        REQUIRE(from_ptr == meta::ucast<From*>(std::addressof(value)));

        using to_void_cv = meta::detail::add_cv_t<To, void>;
        to_void_cv* to_void_ptr = dynamic_cast<to_void_cv*>(from_ptr);
        REQUIRE(to_void_ptr == meta::ucast<to_void_cv*>(from_ptr));

        To* to_ptr = dynamic_cast<To*>(from_ptr);
        REQUIRE(to_ptr == meta::ucast<To*>(from_ptr));

        if ( from_ptr ) {
            if ( to_ptr ) {
                REQUIRE(to_ptr == std::addressof(meta::ucast<To&>(*from_ptr)));
            } else {
                REQUIRE_THROWS(meta::ucast<To&>(*from_ptr));
            }
        }
    }

    template < meta::detail::class_kind From, meta::detail::class_kind To, typename Value >
    void check_casts(Value& value) {
        check_casts_impl<From, To>(value);
        check_casts_impl<const From, const To>(std::as_const(value));
    }
}

TEST_CASE("meta/meta_utilities/ucast/_") {
    namespace meta = meta_hpp;

    meta::class_<A1>();
    meta::class_<B1>().base_<A1>();
    meta::class_<C1>().base_<B1>();

    meta::class_<A2>();
    meta::class_<B2>().base_<A2>();
    meta::class_<C2>().base_<A2>();
    meta::class_<D2>();
    meta::class_<E2>().base_<D2>();
    meta::class_<F2>().base_<B2, C2, E2>();
}

TEST_CASE("meta/meta_utilities/ucast") {
    namespace meta = meta_hpp;

    SUBCASE("1") {
        {
            C1 c;

            check_casts<A1, A1>(c);
            check_casts<A1, B1>(c);
            check_casts<A1, C1>(c);

            check_casts<B1, A1>(c);
            check_casts<B1, B1>(c);
            check_casts<B1, C1>(c);

            check_casts<C1, A1>(c);
            check_casts<C1, B1>(c);
            check_casts<C1, C1>(c);
        }
        {
            B1 b;

            check_casts<A1, A1>(b);
            check_casts<A1, B1>(b);
            check_casts<A1, C1>(b);

            check_casts<B1, A1>(b);
            check_casts<B1, B1>(b);
            check_casts<B1, C1>(b);

            check_casts<C1, A1>(b);
            check_casts<C1, B1>(b);
            check_casts<C1, C1>(b);
        }
        {
            A1 a;

            check_casts<A1, A1>(a);
            check_casts<A1, B1>(a);
            check_casts<A1, C1>(a);

            check_casts<B1, A1>(a);
            check_casts<B1, B1>(a);
            check_casts<B1, C1>(a);

            check_casts<C1, A1>(a);
            check_casts<C1, B1>(a);
            check_casts<C1, C1>(a);
        }
    }

    SUBCASE("2") {
        {
            F2 f;

            check_casts<A2, A2>(f);
            check_casts<A2, B2>(f);
            check_casts<A2, C2>(f);
            check_casts<A2, D2>(f);
            check_casts<A2, E2>(f);
            check_casts<A2, F2>(f);

            check_casts<B2, A2>(f);
            check_casts<B2, B2>(f);
            check_casts<B2, C2>(f);
            check_casts<B2, D2>(f);
            check_casts<B2, E2>(f);
            check_casts<B2, F2>(f);

            check_casts<C2, A2>(f);
            check_casts<C2, B2>(f);
            check_casts<C2, C2>(f);
            check_casts<C2, D2>(f);
            check_casts<C2, E2>(f);
            check_casts<C2, F2>(f);

            check_casts<D2, A2>(f);
            check_casts<D2, B2>(f);
            check_casts<D2, C2>(f);
            check_casts<D2, D2>(f);
            check_casts<D2, E2>(f);
            check_casts<D2, F2>(f);

            check_casts<E2, A2>(f);
            check_casts<E2, B2>(f);
            check_casts<E2, C2>(f);
            check_casts<E2, D2>(f);
            check_casts<E2, E2>(f);
            check_casts<E2, F2>(f);

            check_casts<F2, A2>(f);
            check_casts<F2, B2>(f);
            check_casts<F2, C2>(f);
            check_casts<F2, D2>(f);
            check_casts<F2, E2>(f);
            check_casts<F2, F2>(f);
        }
        {
            B2 b;

            check_casts<A2, A2>(b);
            check_casts<A2, B2>(b);
            check_casts<A2, C2>(b);
            check_casts<A2, D2>(b);
            check_casts<A2, E2>(b);
            check_casts<A2, F2>(b);

            check_casts<B2, A2>(b);
            check_casts<B2, B2>(b);
            check_casts<B2, C2>(b);
            check_casts<B2, D2>(b);
            check_casts<B2, E2>(b);
            check_casts<B2, F2>(b);

            check_casts<C2, A2>(b);
            check_casts<C2, B2>(b);
            check_casts<C2, C2>(b);
            check_casts<C2, D2>(b);
            check_casts<C2, E2>(b);
            check_casts<C2, F2>(b);

            check_casts<D2, A2>(b);
            check_casts<D2, B2>(b);
            check_casts<D2, C2>(b);
            check_casts<D2, D2>(b);
            check_casts<D2, E2>(b);
            check_casts<D2, F2>(b);

            check_casts<E2, A2>(b);
            check_casts<E2, B2>(b);
            check_casts<E2, C2>(b);
            check_casts<E2, D2>(b);
            check_casts<E2, E2>(b);
            check_casts<E2, F2>(b);

            check_casts<F2, A2>(b);
            check_casts<F2, B2>(b);
            check_casts<F2, C2>(b);
            check_casts<F2, D2>(b);
            check_casts<F2, E2>(b);
            check_casts<F2, F2>(b);
        }
        {
            E2 e;

            check_casts<A2, A2>(e);
            check_casts<A2, B2>(e);
            check_casts<A2, C2>(e);
            check_casts<A2, D2>(e);
            check_casts<A2, E2>(e);
            check_casts<A2, F2>(e);

            check_casts<B2, A2>(e);
            check_casts<B2, B2>(e);
            check_casts<B2, C2>(e);
            check_casts<B2, D2>(e);
            check_casts<B2, E2>(e);
            check_casts<B2, F2>(e);

            check_casts<C2, A2>(e);
            check_casts<C2, B2>(e);
            check_casts<C2, C2>(e);
            check_casts<C2, D2>(e);
            check_casts<C2, E2>(e);
            check_casts<C2, F2>(e);

            check_casts<D2, A2>(e);
            check_casts<D2, B2>(e);
            check_casts<D2, C2>(e);
            check_casts<D2, D2>(e);
            check_casts<D2, E2>(e);
            check_casts<D2, F2>(e);

            check_casts<E2, A2>(e);
            check_casts<E2, B2>(e);
            check_casts<E2, C2>(e);
            check_casts<E2, D2>(e);
            check_casts<E2, E2>(e);
            check_casts<E2, F2>(e);

            check_casts<F2, A2>(e);
            check_casts<F2, B2>(e);
            check_casts<F2, C2>(e);
            check_casts<F2, D2>(e);
            check_casts<F2, E2>(e);
            check_casts<F2, F2>(e);
        }
        {
            D2 d;

            check_casts<A2, A2>(d);
            check_casts<A2, B2>(d);
            check_casts<A2, C2>(d);
            check_casts<A2, D2>(d);
            check_casts<A2, E2>(d);
            check_casts<A2, F2>(d);

            check_casts<B2, A2>(d);
            check_casts<B2, B2>(d);
            check_casts<B2, C2>(d);
            check_casts<B2, D2>(d);
            check_casts<B2, E2>(d);
            check_casts<B2, F2>(d);

            check_casts<C2, A2>(d);
            check_casts<C2, B2>(d);
            check_casts<C2, C2>(d);
            check_casts<C2, D2>(d);
            check_casts<C2, E2>(d);
            check_casts<C2, F2>(d);

            check_casts<D2, A2>(d);
            check_casts<D2, B2>(d);
            check_casts<D2, C2>(d);
            check_casts<D2, D2>(d);
            check_casts<D2, E2>(d);
            check_casts<D2, F2>(d);

            check_casts<E2, A2>(d);
            check_casts<E2, B2>(d);
            check_casts<E2, C2>(d);
            check_casts<E2, D2>(d);
            check_casts<E2, E2>(d);
            check_casts<E2, F2>(d);

            check_casts<F2, A2>(d);
            check_casts<F2, B2>(d);
            check_casts<F2, C2>(d);
            check_casts<F2, D2>(d);
            check_casts<F2, E2>(d);
            check_casts<F2, F2>(d);
        }
    }
}
