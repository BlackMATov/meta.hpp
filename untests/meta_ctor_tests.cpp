/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_class.hpp>
#include <meta.hpp/meta_ctor.hpp>

#include "doctest/doctest.hpp"

namespace
{
    class clazz {
    public:
        int v1{1};
        int v2{2};

        clazz() = default;
        clazz(int v): v1{v}, v2{v} {}
        clazz(int v1, int v2): v1{v1}, v2{v2} {}
    };
}

TEST_CASE("meta/ctor") {
    namespace meta = meta_hpp;

    const meta::ctor_info ctor0 = meta::ctor_<>().make_info<clazz>();
    const meta::ctor_info ctor1 = meta::ctor_<int>().make_info<clazz>();
    const meta::ctor_info ctor2 = meta::ctor_<int, int>().make_info<clazz>();

    SUBCASE("arity") {
        CHECK(ctor0.arity() == 0u);
        CHECK(ctor1.arity() == 1u);
        CHECK(ctor2.arity() == 2u);
    }

    SUBCASE("return_type") {
        CHECK(ctor0.return_type() == meta::get_family_id<clazz>());
        CHECK(ctor1.return_type() == meta::get_family_id<clazz>());
        CHECK(ctor2.return_type() == meta::get_family_id<clazz>());
    }

    SUBCASE("argument_types") {
        CHECK_FALSE(ctor0.argument_type(0u));

        CHECK(ctor1.argument_type(0u) == meta::get_family_id<int>());
        CHECK_FALSE(ctor1.argument_type(1u));

        CHECK(ctor2.argument_type(0u) == meta::get_family_id<int>());
        CHECK(ctor2.argument_type(1u) == meta::get_family_id<int>());
        CHECK_FALSE(ctor2.argument_type(2u));
    }

    SUBCASE("invoke0") {
        CHECK(ctor0.invoke().cast<const clazz&>().v1 == 1);
        CHECK(ctor0.invoke().cast<const clazz&>().v2 == 2);
        CHECK_THROWS_AS(ctor0.invoke(1), std::logic_error);
    }

    SUBCASE("invoke1") {
        CHECK(ctor1.invoke(42).cast<const clazz&>().v1 == 42);
        CHECK(ctor1.invoke(42).cast<const clazz&>().v2 == 42);
        CHECK_THROWS_AS(ctor1.invoke(), std::logic_error);
        CHECK_THROWS_AS(ctor1.invoke(1,1), std::logic_error);
    }

    SUBCASE("invoke2") {
        CHECK(ctor2.invoke(21,42).cast<const clazz&>().v1 == 21);
        CHECK(ctor2.invoke(21,42).cast<const clazz&>().v2 == 42);
        CHECK_THROWS_AS(ctor2.invoke(), std::logic_error);
        CHECK_THROWS_AS(ctor2.invoke(1), std::logic_error);
        CHECK_THROWS_AS(ctor2.invoke(1,1,1), std::logic_error);
    }

    SUBCASE("is_invocable_with") {
        CHECK(ctor0.is_invocable_with<>());
        CHECK_FALSE(ctor0.is_invocable_with<int>());

        CHECK(ctor1.is_invocable_with<int>());
        CHECK_FALSE(ctor1.is_invocable_with<float>());
        CHECK_FALSE(ctor1.is_invocable_with<>());
        CHECK_FALSE(ctor1.is_invocable_with<int, int>());

        CHECK(ctor2.is_invocable_with<int, int>());
        CHECK_FALSE(ctor2.is_invocable_with<int, float>());
        CHECK_FALSE(ctor2.is_invocable_with<>());
        CHECK_FALSE(ctor2.is_invocable_with<int>());
    }
}

TEST_CASE("meta/ctor/class") {
    namespace meta = meta_hpp;

    const meta::class_info clazz_info = meta::class_<clazz>("clazz")(
        meta::ctor_<>(),
        meta::ctor_<int>(),
        meta::ctor_<int, int>()
    ).make_info();

    CHECK(clazz_info.get_ctor<>());
    CHECK(clazz_info.get_ctor<int>());
    CHECK(clazz_info.get_ctor<int, int>());

    CHECK_FALSE(clazz_info.get_ctor<float>());
    CHECK_FALSE(clazz_info.get_ctor<int, float>());
    CHECK_FALSE(clazz_info.get_ctor<int, int, int>());
}
