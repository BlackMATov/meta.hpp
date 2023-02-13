/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
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

    struct B : virtual A {};
    struct C : virtual A {};
    struct D : B, C {};
    struct E {};
}

TEST_CASE("meta/meta_features/diamond") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::uinst;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    meta::class_<A>();
    meta::class_<B>().base_<A>();
    meta::class_<C>().base_<A>();
    meta::class_<D>().base_<B>().base_<C>();
    meta::class_<E>();

    //  * <- B <- *
    // A           D
    //  * <- C <- *

    const meta::class_type A_type = meta::resolve_type<A>();
    const meta::class_type B_type = meta::resolve_type<B>();
    const meta::class_type C_type = meta::resolve_type<C>();
    const meta::class_type D_type = meta::resolve_type<D>();
    const meta::class_type E_type = meta::resolve_type<E>();

    REQUIRE(A_type);
    REQUIRE(B_type);
    REQUIRE(C_type);
    REQUIRE(D_type);
    REQUIRE(E_type);

    SUBCASE("is_base_of") {
        CHECK(!A_type.is_base_of(A_type));
        CHECK(A_type.is_base_of(B_type));
        CHECK(A_type.is_base_of(C_type));
        CHECK(A_type.is_base_of(D_type));
        CHECK(!A_type.is_base_of(E_type));

        CHECK(!B_type.is_base_of(A_type));
        CHECK(!B_type.is_base_of(B_type));
        CHECK(!B_type.is_base_of(C_type));
        CHECK(B_type.is_base_of(D_type));
        CHECK(!B_type.is_base_of(E_type));

        CHECK(!C_type.is_base_of(A_type));
        CHECK(!C_type.is_base_of(B_type));
        CHECK(!C_type.is_base_of(C_type));
        CHECK(C_type.is_base_of(D_type));
        CHECK(!C_type.is_base_of(E_type));

        CHECK(!D_type.is_base_of(A_type));
        CHECK(!D_type.is_base_of(B_type));
        CHECK(!D_type.is_base_of(C_type));
        CHECK(!D_type.is_base_of(D_type));
        CHECK(!D_type.is_base_of(E_type));

        CHECK(!E_type.is_base_of(A_type));
        CHECK(!E_type.is_base_of(B_type));
        CHECK(!E_type.is_base_of(C_type));
        CHECK(!E_type.is_base_of(D_type));
        CHECK(!E_type.is_base_of(E_type));
    }

    SUBCASE("is_derived_from") {
        CHECK(!A_type.is_derived_from(A_type));
        CHECK(!A_type.is_derived_from(B_type));
        CHECK(!A_type.is_derived_from(C_type));
        CHECK(!A_type.is_derived_from(D_type));
        CHECK(!A_type.is_derived_from(E_type));

        CHECK(B_type.is_derived_from(A_type));
        CHECK(!B_type.is_derived_from(B_type));
        CHECK(!B_type.is_derived_from(C_type));
        CHECK(!B_type.is_derived_from(D_type));
        CHECK(!B_type.is_derived_from(E_type));

        CHECK(C_type.is_derived_from(A_type));
        CHECK(!C_type.is_derived_from(B_type));
        CHECK(!C_type.is_derived_from(C_type));
        CHECK(!C_type.is_derived_from(D_type));
        CHECK(!C_type.is_derived_from(E_type));

        CHECK(D_type.is_derived_from(A_type));
        CHECK(D_type.is_derived_from(B_type));
        CHECK(D_type.is_derived_from(C_type));
        CHECK(!D_type.is_derived_from(D_type));
        CHECK(!D_type.is_derived_from(E_type));

        CHECK(!E_type.is_derived_from(A_type));
        CHECK(!E_type.is_derived_from(B_type));
        CHECK(!E_type.is_derived_from(C_type));
        CHECK(!E_type.is_derived_from(D_type));
        CHECK(!E_type.is_derived_from(E_type));
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
        }
        {
            const B b;
            CHECK(pointer_upcast<A>(r, &b) == &b);
            CHECK(pointer_upcast<B>(r, &b) == &b);
            CHECK_FALSE(pointer_upcast<C>(r, &b));
            CHECK_FALSE(pointer_upcast<D>(r, &b));
            CHECK_FALSE(pointer_upcast<E>(r, &b));
        }
        {
            C c;
            CHECK(pointer_upcast<A>(r, &c) == &c);
            CHECK_FALSE(pointer_upcast<B>(r, &c));
            CHECK(pointer_upcast<C>(r, &c) == &c);
            CHECK_FALSE(pointer_upcast<D>(r, &c));
            CHECK_FALSE(pointer_upcast<E>(r, &c));
        }
        {
            const D d;
            CHECK(pointer_upcast<A>(r, &d) == &d);
            CHECK(pointer_upcast<B>(r, &d) == &d);
            CHECK(pointer_upcast<C>(r, &d) == &d);
            CHECK(pointer_upcast<D>(r, &d) == &d);
            CHECK_FALSE(pointer_upcast<E>(r, &d));
        }
        {
            E e;
            CHECK_FALSE(pointer_upcast<A>(r, &e));
            CHECK_FALSE(pointer_upcast<B>(r, &e));
            CHECK_FALSE(pointer_upcast<C>(r, &e));
            CHECK_FALSE(pointer_upcast<D>(r, &e));
            CHECK(pointer_upcast<E>(r, &e) == &e);
        }
    }

    SUBCASE("arg/cast") {
        {
            A a;
            meta::uvalue a_val{&a};
            CHECK(*static_cast<A**>(a_val.get_data()) == &a);

            uarg a_arg{r, a_val};

            CHECK(a_arg.can_cast_to<A*>(r));
            CHECK(!a_arg.can_cast_to<B*>(r));
            CHECK(!a_arg.can_cast_to<C*>(r));
            CHECK(!a_arg.can_cast_to<D*>(r));
            CHECK(!a_arg.can_cast_to<E*>(r));

            CHECK(a_arg.cast<A*>(r) == static_cast<A*>(&a));
        }
        {
            B b;
            meta::uvalue b_val{&b};
            CHECK(*static_cast<B**>(b_val.get_data()) == &b);

            uarg b_arg{r, b_val};

            CHECK(b_arg.can_cast_to<A*>(r));
            CHECK(b_arg.can_cast_to<B*>(r));
            CHECK(!b_arg.can_cast_to<C*>(r));
            CHECK(!b_arg.can_cast_to<D*>(r));
            CHECK(!b_arg.can_cast_to<E*>(r));

            CHECK(b_arg.cast<A*>(r) == static_cast<A*>(&b));
            CHECK(b_arg.cast<B*>(r) == static_cast<B*>(&b));
        }
        {
            C c;
            meta::uvalue c_val{&c};
            CHECK(*static_cast<C**>(c_val.get_data()) == &c);

            uarg c_arg{r, c_val};

            CHECK(c_arg.can_cast_to<A*>(r));
            CHECK(!c_arg.can_cast_to<B*>(r));
            CHECK(c_arg.can_cast_to<C*>(r));
            CHECK(!c_arg.can_cast_to<D*>(r));
            CHECK(!c_arg.can_cast_to<E*>(r));

            CHECK(c_arg.cast<A*>(r) == static_cast<A*>(&c));
            CHECK(c_arg.cast<C*>(r) == static_cast<C*>(&c));
        }
        {
            D d;
            meta::uvalue d_val{&d};
            CHECK(*static_cast<D**>(d_val.get_data()) == &d);

            uarg d_arg{r, d_val};

            CHECK(d_arg.can_cast_to<A*>(r));
            CHECK(d_arg.can_cast_to<B*>(r));
            CHECK(d_arg.can_cast_to<C*>(r));
            CHECK(d_arg.can_cast_to<D*>(r));
            CHECK(!d_arg.can_cast_to<E*>(r));

            CHECK(d_arg.cast<A*>(r) == static_cast<A*>(&d));
            CHECK(d_arg.cast<B*>(r) == static_cast<B*>(&d));
            CHECK(d_arg.cast<C*>(r) == static_cast<C*>(&d));
            CHECK(d_arg.cast<D*>(r) == static_cast<D*>(&d));
        }
        {
            E e;
            meta::uvalue e_val{&e};
            CHECK(*static_cast<E**>(e_val.get_data()) == &e);

            uarg e_arg{r, e_val};

            CHECK(!e_arg.can_cast_to<A*>(r));
            CHECK(!e_arg.can_cast_to<B*>(r));
            CHECK(!e_arg.can_cast_to<C*>(r));
            CHECK(!e_arg.can_cast_to<D*>(r));
            CHECK(e_arg.can_cast_to<E*>(r));

            CHECK(e_arg.cast<E*>(r) == static_cast<E*>(&e));
        }
    }

    SUBCASE("inst/cast") {
        {
            meta::uvalue a_val{A{}};
            uinst a_inst{r, a_val};

            CHECK(a_inst.can_cast_to<A&>(r));
            CHECK_FALSE(a_inst.can_cast_to<B&>(r));
            CHECK_FALSE(a_inst.can_cast_to<C&>(r));
            CHECK_FALSE(a_inst.can_cast_to<D&>(r));
            CHECK_FALSE(a_inst.can_cast_to<E&>(r));

            CHECK(&a_inst.cast<A&>(r) == &a_val.get_as<A>());
        }
        {
            meta::uvalue b_val{B{}};
            uinst b_inst{r, b_val};

            CHECK(b_inst.can_cast_to<A&>(r));
            CHECK(b_inst.can_cast_to<B&>(r));
            CHECK_FALSE(b_inst.can_cast_to<C&>(r));
            CHECK_FALSE(b_inst.can_cast_to<D&>(r));
            CHECK_FALSE(b_inst.can_cast_to<E&>(r));

            CHECK(&b_inst.cast<A&>(r) == &b_val.get_as<B>());
            CHECK(&b_inst.cast<B&>(r) == &b_val.get_as<B>());

            CHECK(&b_inst.cast<A&>(r) == &b_val.get_as<B>());
        }
        {
            meta::uvalue c_val{C{}};
            uinst c_inst{r, c_val};

            CHECK(c_inst.can_cast_to<A&>(r));
            CHECK_FALSE(c_inst.can_cast_to<B&>(r));
            CHECK(c_inst.can_cast_to<C&>(r));
            CHECK_FALSE(c_inst.can_cast_to<D&>(r));
            CHECK_FALSE(c_inst.can_cast_to<E&>(r));

            CHECK(&c_inst.cast<A&>(r) == &c_val.get_as<C>());
            CHECK(&c_inst.cast<C&>(r) == &c_val.get_as<C>());
        }
        {
            meta::uvalue d_val{D{}};
            uinst d_inst{r, d_val};

            CHECK(d_inst.can_cast_to<A&>(r));
            CHECK(d_inst.can_cast_to<B&>(r));
            CHECK(d_inst.can_cast_to<C&>(r));
            CHECK(d_inst.can_cast_to<D&>(r));
            CHECK_FALSE(d_inst.can_cast_to<E&>(r));

            CHECK(&d_inst.cast<A&>(r) == &d_val.get_as<D>());
            CHECK(&d_inst.cast<B&>(r) == &d_val.get_as<D>());
            CHECK(&d_inst.cast<C&>(r) == &d_val.get_as<D>());
            CHECK(&d_inst.cast<D&>(r) == &d_val.get_as<D>());

            CHECK(&d_inst.cast<A&>(r) == &d_val.get_as<D>());
            CHECK(&d_inst.cast<B&>(r) == &d_val.get_as<D>());
            CHECK(&d_inst.cast<C&>(r) == &d_val.get_as<D>());
            CHECK(&d_inst.cast<D&>(r) == &d_val.get_as<D>());
        }
        {
            meta::uvalue e_val{E{}};
            uinst e_inst{r, e_val};

            CHECK_FALSE(e_inst.can_cast_to<A&>(r));
            CHECK_FALSE(e_inst.can_cast_to<B&>(r));
            CHECK_FALSE(e_inst.can_cast_to<C&>(r));
            CHECK_FALSE(e_inst.can_cast_to<D&>(r));
            CHECK(e_inst.can_cast_to<E&>(r));

            CHECK(&e_inst.cast<E&>(r) == &e_val.get_as<E>());
        }
    }

    SUBCASE("resolve_polymorphic_type") {
        const D d;

        const A& ad = d;
        const B& bd = d;
        const C& cd = d;
        const D& dd = d;

        CHECK(meta::resolve_type(ad) == meta::resolve_type<A>());
        CHECK(meta::resolve_type(bd) == meta::resolve_type<B>());
        CHECK(meta::resolve_type(cd) == meta::resolve_type<C>());
        CHECK(meta::resolve_type(dd) == meta::resolve_type<D>());

    #if !defined(META_HPP_NO_RTTI)
        CHECK(meta::resolve_polymorphic_type(ad) == meta::resolve_type<D>());
        CHECK(meta::resolve_polymorphic_type(bd) == meta::resolve_type<D>());
        CHECK(meta::resolve_polymorphic_type(cd) == meta::resolve_type<D>());
        CHECK(meta::resolve_polymorphic_type(dd) == meta::resolve_type<D>());
    #endif
    }
}
