/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

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
            CHECK(pointer_upcast<A>(&a) == &a);
            CHECK_FALSE(pointer_upcast<B>(&a));
            CHECK_FALSE(pointer_upcast<C>(&a));
            CHECK_FALSE(pointer_upcast<D>(&a));
            CHECK_FALSE(pointer_upcast<E>(&a));
        }
        {
            const B b;
            CHECK(pointer_upcast<A>(&b) == &b);
            CHECK(pointer_upcast<B>(&b) == &b);
            CHECK_FALSE(pointer_upcast<C>(&b));
            CHECK_FALSE(pointer_upcast<D>(&b));
            CHECK_FALSE(pointer_upcast<E>(&b));
        }
        {
            C c;
            CHECK(pointer_upcast<A>(&c) == &c);
            CHECK_FALSE(pointer_upcast<B>(&c));
            CHECK(pointer_upcast<C>(&c) == &c);
            CHECK_FALSE(pointer_upcast<D>(&c));
            CHECK_FALSE(pointer_upcast<E>(&c));
        }
        {
            const D d;
            CHECK(pointer_upcast<A>(&d) == &d);
            CHECK(pointer_upcast<B>(&d) == &d);
            CHECK(pointer_upcast<C>(&d) == &d);
            CHECK(pointer_upcast<D>(&d) == &d);
            CHECK_FALSE(pointer_upcast<E>(&d));
        }
        {
            E e;
            CHECK_FALSE(pointer_upcast<A>(&e));
            CHECK_FALSE(pointer_upcast<B>(&e));
            CHECK_FALSE(pointer_upcast<C>(&e));
            CHECK_FALSE(pointer_upcast<D>(&e));
            CHECK(pointer_upcast<E>(&e) == &e);
        }
    }

    SUBCASE("arg/cast") {
        {
            A a;
            meta::value a_val{&a};
            CHECK(*static_cast<A**>(a_val.data()) == &a);

            meta::detail::arg a_arg{a_val};

            CHECK(a_arg.can_cast_to<A*>());
            CHECK(!a_arg.can_cast_to<B*>());
            CHECK(!a_arg.can_cast_to<C*>());
            CHECK(!a_arg.can_cast_to<D*>());
            CHECK(!a_arg.can_cast_to<E*>());

            CHECK(a_arg.cast<A*>() == static_cast<A*>(&a));
        }
        {
            B b;
            meta::value b_val{&b};
            CHECK(*static_cast<B**>(b_val.data()) == &b);

            meta::detail::arg b_arg{b_val};

            CHECK(b_arg.can_cast_to<A*>());
            CHECK(b_arg.can_cast_to<B*>());
            CHECK(!b_arg.can_cast_to<C*>());
            CHECK(!b_arg.can_cast_to<D*>());
            CHECK(!b_arg.can_cast_to<E*>());

            CHECK(b_arg.cast<A*>() == static_cast<A*>(&b));
            CHECK(b_arg.cast<B*>() == static_cast<B*>(&b));
        }
        {
            C c;
            meta::value c_val{&c};
            CHECK(*static_cast<C**>(c_val.data()) == &c);

            meta::detail::arg c_arg{c_val};

            CHECK(c_arg.can_cast_to<A*>());
            CHECK(!c_arg.can_cast_to<B*>());
            CHECK(c_arg.can_cast_to<C*>());
            CHECK(!c_arg.can_cast_to<D*>());
            CHECK(!c_arg.can_cast_to<E*>());

            CHECK(c_arg.cast<A*>() == static_cast<A*>(&c));
            CHECK(c_arg.cast<C*>() == static_cast<C*>(&c));
        }
        {
            D d;
            meta::value d_val{&d};
            CHECK(*static_cast<D**>(d_val.data()) == &d);

            meta::detail::arg d_arg{d_val};

            CHECK(d_arg.can_cast_to<A*>());
            CHECK(d_arg.can_cast_to<B*>());
            CHECK(d_arg.can_cast_to<C*>());
            CHECK(d_arg.can_cast_to<D*>());
            CHECK(!d_arg.can_cast_to<E*>());

            CHECK(d_arg.cast<A*>() == static_cast<A*>(&d));
            CHECK(d_arg.cast<B*>() == static_cast<B*>(&d));
            CHECK(d_arg.cast<C*>() == static_cast<C*>(&d));
            CHECK(d_arg.cast<D*>() == static_cast<D*>(&d));
        }
        {
            E e;
            meta::value e_val{&e};
            CHECK(*static_cast<E**>(e_val.data()) == &e);

            meta::detail::arg e_arg{e_val};

            CHECK(!e_arg.can_cast_to<A*>());
            CHECK(!e_arg.can_cast_to<B*>());
            CHECK(!e_arg.can_cast_to<C*>());
            CHECK(!e_arg.can_cast_to<D*>());
            CHECK(e_arg.can_cast_to<E*>());

            CHECK(e_arg.cast<E*>() == static_cast<E*>(&e));
        }
    }

    SUBCASE("inst/cast") {
        {
            meta::value a_val{A{}};
            meta::detail::inst a_inst{a_val};

            CHECK(a_inst.can_cast_to<A&>());
            CHECK_FALSE(a_inst.can_cast_to<B&>());
            CHECK_FALSE(a_inst.can_cast_to<C&>());
            CHECK_FALSE(a_inst.can_cast_to<D&>());
            CHECK_FALSE(a_inst.can_cast_to<E&>());

            CHECK(&a_inst.cast<A&>() == a_val.try_cast<A>());
        }
        {
            meta::value b_val{B{}};
            meta::detail::inst b_inst{b_val};

            CHECK(b_inst.can_cast_to<A&>());
            CHECK(b_inst.can_cast_to<B&>());
            CHECK_FALSE(b_inst.can_cast_to<C&>());
            CHECK_FALSE(b_inst.can_cast_to<D&>());
            CHECK_FALSE(b_inst.can_cast_to<E&>());

            CHECK(&b_inst.cast<A&>() == b_val.try_cast<B>());
            CHECK(&b_inst.cast<B&>() == b_val.try_cast<B>());

            CHECK(&b_inst.cast<A&>() == b_val.try_cast<B>());
        }
        {
            meta::value c_val{C{}};
            meta::detail::inst c_inst{c_val};

            CHECK(c_inst.can_cast_to<A&>());
            CHECK_FALSE(c_inst.can_cast_to<B&>());
            CHECK(c_inst.can_cast_to<C&>());
            CHECK_FALSE(c_inst.can_cast_to<D&>());
            CHECK_FALSE(c_inst.can_cast_to<E&>());

            CHECK(&c_inst.cast<A&>() == c_val.try_cast<C>());
            CHECK(&c_inst.cast<C&>() == c_val.try_cast<C>());
        }
        {
            meta::value d_val{D{}};
            meta::detail::inst d_inst{d_val};

            CHECK(d_inst.can_cast_to<A&>());
            CHECK(d_inst.can_cast_to<B&>());
            CHECK(d_inst.can_cast_to<C&>());
            CHECK(d_inst.can_cast_to<D&>());
            CHECK_FALSE(d_inst.can_cast_to<E&>());

            CHECK(&d_inst.cast<A&>() == d_val.try_cast<D>());
            CHECK(&d_inst.cast<B&>() == d_val.try_cast<D>());
            CHECK(&d_inst.cast<C&>() == d_val.try_cast<D>());
            CHECK(&d_inst.cast<D&>() == d_val.try_cast<D>());

            CHECK(&d_inst.cast<A&>() == d_val.try_cast<D>());
            CHECK(&d_inst.cast<B&>() == d_val.try_cast<D>());
            CHECK(&d_inst.cast<C&>() == d_val.try_cast<D>());
            CHECK(&d_inst.cast<D&>() == d_val.try_cast<D>());
        }
        {
            meta::value e_val{E{}};
            meta::detail::inst e_inst{e_val};

            CHECK_FALSE(e_inst.can_cast_to<A&>());
            CHECK_FALSE(e_inst.can_cast_to<B&>());
            CHECK_FALSE(e_inst.can_cast_to<C&>());
            CHECK_FALSE(e_inst.can_cast_to<D&>());
            CHECK(e_inst.can_cast_to<E&>());

            CHECK(&e_inst.cast<E&>() == e_val.try_cast<E>());
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

        CHECK(meta::resolve_polymorphic_type(ad) == meta::resolve_type<D>());
        CHECK(meta::resolve_polymorphic_type(bd) == meta::resolve_type<D>());
        CHECK(meta::resolve_polymorphic_type(cd) == meta::resolve_type<D>());
        CHECK(meta::resolve_polymorphic_type(dd) == meta::resolve_type<D>());
    }
}
