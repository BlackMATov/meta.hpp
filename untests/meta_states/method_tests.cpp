/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct base {
        virtual ~base() = default;
        virtual int pure_virtual_method() = 0;
    };

    struct clazz : base {
        clazz() = default;

        clazz(clazz&&) = delete;
        clazz(const clazz&) = delete;

        clazz& operator=(clazz&&) = delete;
        clazz& operator=(const clazz&) = delete;

        //

        int non_const_method() { return 1; }
        int non_const_method_noexcept() noexcept { return 2; }

        int const_method() const { return 3; }
        int const_method_noexcept() const noexcept { return 4; }

        int non_const_method_ref() & { return 5; }
        int non_const_method_noexcept_ref() & noexcept { return 6; }

        int const_method_ref() const & { return 7; }
        int const_method_noexcept_ref() const & noexcept { return 8; }

        int non_const_method_rref() && { return 9; }
        int non_const_method_noexcept_rref() && noexcept { return 10; }

        int const_method_rref() const && { return 11; }
        int const_method_noexcept_rref() const && noexcept { return 12; }

        //

        int pure_virtual_method() override { return -1; }
    };

    struct derived_clazz : clazz {
        int pure_virtual_method() override { return -2; }
    };

    struct clazz2 {};
}

TEST_CASE("meta/meta_states/method") {
    namespace meta = meta_hpp;

    meta::class_<base>()
        .method_("pure_virtual_method", &base::pure_virtual_method);

    meta::class_<clazz>()
        .base_<base>()

        .method_("non_const_method", &clazz::non_const_method)
        .method_("non_const_method_noexcept", &clazz::non_const_method_noexcept)

        .method_("const_method", &clazz::const_method)
        .method_("const_method_noexcept", &clazz::const_method_noexcept)

        .method_("non_const_method_ref", &clazz::non_const_method_ref)
        .method_("non_const_method_noexcept_ref", &clazz::non_const_method_noexcept_ref)

        .method_("const_method_ref", &clazz::const_method_ref)
        .method_("const_method_noexcept_ref", &clazz::const_method_noexcept_ref)

        .method_("non_const_method_rref", &clazz::non_const_method_rref)
        .method_("non_const_method_noexcept_rref", &clazz::non_const_method_noexcept_rref)

        .method_("const_method_rref", &clazz::const_method_rref)
        .method_("const_method_noexcept_rref", &clazz::const_method_noexcept_rref);

    meta::class_<derived_clazz>()
        .base_<clazz>();

    const meta::class_type ct = meta::resolve_type<clazz>();
    REQUIRE(ct);

    SUBCASE("") {
        const meta::method method;
        CHECK_FALSE(method);
        CHECK_FALSE(method.is_valid());
        CHECK(method == ct.get_method("non-existent-method"));
    }

    SUBCASE("operators") {
        const meta::method non_const_method_m = ct.get_method("non_const_method");
        const meta::method non_const_method_rref_m = ct.get_method("non_const_method_rref");
        CHECK(non_const_method_m == non_const_method_m);
        CHECK(non_const_method_m != non_const_method_rref_m);
        CHECK((non_const_method_m < non_const_method_rref_m || non_const_method_rref_m < non_const_method_m));
    }

    SUBCASE("non_const_method") {
        const meta::method mi = ct.get_method("non_const_method");
        REQUIRE(mi);

        CHECK(mi.get_name() == "non_const_method");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == meta::method_flags{});
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 1);
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl)) == 1);
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method), derived_clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method), const derived_clazz&&>);
    }

    SUBCASE("non_const_method_noexcept") {
        const meta::method mi = ct.get_method("non_const_method_noexcept");
        REQUIRE(mi);

        CHECK(mi.get_name() == "non_const_method_noexcept");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == meta::method_flags::is_noexcept);
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 2);
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl)) == 2);
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), derived_clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), const derived_clazz&&>);
    }

    SUBCASE("const_method") {
        const meta::method mi = ct.get_method("const_method");
        REQUIRE(mi);

        CHECK(mi.get_name() == "const_method");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == meta::method_flags::is_const);
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 3);
            CHECK(mi.invoke(std::as_const(cl)) == 3);
            CHECK(mi.invoke(std::move(cl)) == 3);
            CHECK(mi.invoke(std::move(std::as_const(cl))) == 3);
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method), derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), derived_clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), const derived_clazz&&>);
    }

    SUBCASE("const_method_noexcept") {
        const meta::method mi = ct.get_method("const_method_noexcept");
        REQUIRE(mi);

        CHECK(mi.get_name() == "const_method_noexcept");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == (meta::method_flags::is_const | meta::method_flags::is_noexcept));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 4);
            CHECK(mi.invoke(std::as_const(cl)) == 4);
            CHECK(mi.invoke(std::move(cl)) == 4);
            CHECK(mi.invoke(std::move(std::as_const(cl))) == 4);
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), derived_clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), const derived_clazz&&>);
    }

    SUBCASE("non_const_method_ref") {
        const meta::method mi = ct.get_method("non_const_method_ref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "non_const_method_ref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == meta::method_flags::is_lvalue_qualified);
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<derived_clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 5);
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK_THROWS(mi.invoke(std::move(cl)));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_ref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_ref), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), const derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), derived_clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), const derived_clazz&&>);
    }

    SUBCASE("non_const_method_noexcept_ref") {
        const meta::method mi = ct.get_method("non_const_method_noexcept_ref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "non_const_method_noexcept_ref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == (meta::method_flags::is_noexcept | meta::method_flags::is_lvalue_qualified));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<derived_clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 6);
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK_THROWS(mi.invoke(std::move(cl)));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), const derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), derived_clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), const derived_clazz&&>);
    }

    SUBCASE("const_method_ref") {
        const meta::method mi = ct.get_method("const_method_ref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "const_method_ref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == (meta::method_flags::is_const | meta::method_flags::is_lvalue_qualified));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 7);
            CHECK(mi.invoke(std::as_const(cl)) == 7);
            CHECK(mi.invoke(std::move(cl)) == 7);
            CHECK(mi.invoke(std::move(std::as_const(cl))) == 7);
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), derived_clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), const derived_clazz&&>);
    }

    SUBCASE("const_method_noexcept_ref") {
        const meta::method mi = ct.get_method("const_method_noexcept_ref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "const_method_noexcept_ref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == (meta::method_flags::is_const | meta::method_flags::is_noexcept | meta::method_flags::is_lvalue_qualified));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl) == 8);
            CHECK(mi.invoke(std::as_const(cl)) == 8);
            CHECK(mi.invoke(std::move(cl)) == 8);
            CHECK(mi.invoke(std::move(std::as_const(cl))) == 8);
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), derived_clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), const derived_clazz&&>);
    }

    SUBCASE("non_const_method_rref") {
        const meta::method mi = ct.get_method("non_const_method_rref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "non_const_method_rref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == meta::method_flags::is_rvalue_qualified);
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl)) == 9);
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_rref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_rref), derived_clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), const derived_clazz&&>);
    }

    SUBCASE("non_const_method_noexcept_rref") {
        const meta::method mi = ct.get_method("non_const_method_noexcept_rref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "non_const_method_noexcept_rref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == (meta::method_flags::is_noexcept | meta::method_flags::is_rvalue_qualified));
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl)) == 10);
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), derived_clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), const derived_clazz&&>);
    }

    SUBCASE("const_method_rref") {
        const meta::method mi = ct.get_method("const_method_rref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "const_method_rref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == (meta::method_flags::is_const | meta::method_flags::is_rvalue_qualified));
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl)) == 11);
            CHECK(mi.invoke(std::move(std::as_const(cl))) == 11);
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref), derived_clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref), const derived_clazz&&>);
    }

    SUBCASE("const_method_noexcept_rref") {
        const meta::method mi = ct.get_method("const_method_noexcept_rref");
        REQUIRE(mi);

        CHECK(mi.get_name() == "const_method_noexcept_rref");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<clazz>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == (meta::method_flags::is_const | meta::method_flags::is_noexcept | meta::method_flags::is_rvalue_qualified));
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi.is_invocable_with<derived_clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const derived_clazz&>());
            CHECK(mi.is_invocable_with<derived_clazz&&>());
            CHECK(mi.is_invocable_with<const derived_clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl)) == 12);
            CHECK(mi.invoke(std::move(std::as_const(cl))) == 12);
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), derived_clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), const derived_clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), derived_clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), const derived_clazz&&>);
    }

    SUBCASE("pure_virtual_method") {
        const meta::method mi = ct.get_method("pure_virtual_method");
        REQUIRE(mi);

        CHECK(mi.get_name() == "pure_virtual_method");

        {
            CHECK(mi.get_type().get_arity() == 0);
            CHECK(mi.get_type().get_owner_type() == meta::resolve_type<base>());
            CHECK(mi.get_type().get_return_type() == meta::resolve_type<int>());
            CHECK(mi.get_type().get_flags() == meta::method_flags{});
        }

        {
            clazz cl;
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.invoke(cl) == -1);
        }

        {
            derived_clazz dcl;
            CHECK(mi.is_invocable_with<derived_clazz&>());
            CHECK(mi.invoke(dcl) == -2);
        }
    }
}
