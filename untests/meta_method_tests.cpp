/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_method.hpp>
#include "doctest/doctest.hpp"

namespace
{
    class clazz {
    public:
        void void_f_void() noexcept {}
        void void_f_int(int a) { (void)a; }
        void void_f_int2(const int& a, int b) { (void)a; (void)b; }

        const int& int_f_void() noexcept { static int i = 1; return i; }
        int int_f_int(const int a) { return a; }
        int int_f_int2(int a, const int& b) { return a + b; }

        void const_void_f_void() const noexcept {}
        void const_void_f_int(int a) const { (void)a; }
        void const_void_f_int2(int a, int b) const { (void)a; (void)b; }

        int const_int_f_void() const noexcept { return 1; }
        int const_int_f_int(int a) const { return a; }
        int const_int_f_int2(int a, int b) const { return a + b; }
    };

    class clazz2 {};
}

TEST_CASE("meta/non_const_method") {
    namespace meta = meta_hpp;

    meta::method_ void_f_void_method_("void_f_void", &clazz::void_f_void);
    meta::method_ void_f_int_method_("void_f_int", &clazz::void_f_int);
    meta::method_ void_f_int2_method_("void_f_int2", &clazz::void_f_int2);

    meta::method_ int_f_void_method_("int_f_void", &clazz::int_f_void);
    meta::method_ int_f_int_method_("int_f_int", &clazz::int_f_int);
    meta::method_ int_f_int2_method_("int_f_int2", &clazz::int_f_int2);

    const meta::method_info& void_f_void_info = void_f_void_method_;
    const meta::method_info& void_f_int_info = void_f_int_method_;
    const meta::method_info& void_f_int2_info = void_f_int2_method_;

    const meta::method_info& int_f_void_info = int_f_void_method_;
    const meta::method_info& int_f_int_info = int_f_int_method_;
    const meta::method_info& int_f_int2_info = int_f_int2_method_;

    SUBCASE("arity") {
        CHECK(void_f_void_info.arity() == 0);
        CHECK(void_f_int_info.arity() == 1);
        CHECK(void_f_int2_info.arity() == 2);

        CHECK(int_f_void_info.arity() == 0);
        CHECK(int_f_int_info.arity() == 1);
        CHECK(int_f_int2_info.arity() == 2);
    }

    SUBCASE("instance_type") {
        CHECK(void_f_void_info.instance_type() == meta::get_family_id<clazz>());
        CHECK(int_f_void_info.instance_type() == meta::get_family_id<clazz>());
    }

    SUBCASE("return_type") {
        CHECK_FALSE(void_f_void_info.return_type());
        CHECK(int_f_void_info.return_type() == meta::get_family_id<int>());
    }

    SUBCASE("argument_types") {
        CHECK_FALSE(void_f_void_info.argument_type(0u));

        CHECK(int_f_int_info.argument_type(0u) == meta::get_family_id<int>());
        CHECK_FALSE(int_f_int_info.argument_type(1u));

        CHECK(int_f_int2_info.argument_type(0u) == meta::get_family_id<int>());
        CHECK(int_f_int2_info.argument_type(1u) == meta::get_family_id<int>());
        CHECK_FALSE(int_f_int2_info.argument_type(2u));
    }

    SUBCASE("another_instance") {
        clazz2 instance;
        CHECK_THROWS_AS(void_f_void_info.invoke(instance), std::logic_error);
        CHECK_THROWS_AS(void_f_int_info.invoke(instance, 1), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1, 2), std::logic_error);
    }

    SUBCASE("void_return") {
        clazz instance;

        CHECK_NOTHROW(void_f_void_info.invoke(instance));
        CHECK_THROWS_AS(void_f_void_info.invoke(instance, 1), std::logic_error);

        CHECK_THROWS_AS(void_f_int_info.invoke(instance), std::logic_error);
        CHECK_NOTHROW(void_f_int_info.invoke(instance, 1));
        CHECK_THROWS_AS(void_f_int_info.invoke(instance, 1.f), std::logic_error);
        CHECK_THROWS_AS(void_f_int_info.invoke(instance, 1, 2), std::logic_error);

        CHECK_THROWS_AS(void_f_int2_info.invoke(instance), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1), std::logic_error);
        CHECK_NOTHROW(void_f_int2_info.invoke(instance, 1, 2));
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1.f, 2), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1, 2.f), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1, 2, 3), std::logic_error);

        const clazz& cinstance = instance;
        CHECK_THROWS_AS(void_f_void_info.invoke(cinstance), std::logic_error);
        CHECK_THROWS_AS(void_f_int_info.invoke(cinstance, 1), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(cinstance, 1, 2), std::logic_error);
    }

    SUBCASE("int_return") {
        clazz instance;

        CHECK(int_f_void_info.invoke(instance)->cast<int>() == 1);
        CHECK_THROWS_AS(int_f_void_info.invoke(instance, 1), std::logic_error);

        CHECK_THROWS_AS(int_f_int_info.invoke(instance), std::logic_error);
        CHECK(int_f_int_info.invoke(instance, 1)->cast<int>() == 1);
        CHECK_THROWS_AS(int_f_int_info.invoke(instance, 1.f), std::logic_error);
        CHECK_THROWS_AS(int_f_int_info.invoke(instance, 1, 2), std::logic_error);

        CHECK_THROWS_AS(int_f_int2_info.invoke(instance), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1), std::logic_error);
        CHECK(int_f_int2_info.invoke(instance, 1, 2)->cast<int>() == 3);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1.f, 2), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1, 2.f), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1, 2, 3), std::logic_error);

        const clazz& cinstance = instance;
        CHECK_THROWS_AS(int_f_void_info.invoke(cinstance), std::logic_error);
        CHECK_THROWS_AS(int_f_int_info.invoke(cinstance, 1), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(cinstance, 1, 2), std::logic_error);
    }

    SUBCASE("int_return_by_value") {
        meta::value instance{clazz{}};
        CHECK(int_f_void_info.invoke(instance)->cast<int>() == 1);
        CHECK(int_f_int_info.invoke(instance, 1)->cast<int>() == 1);
        CHECK(int_f_int2_info.invoke(instance, 1, 2)->cast<int>() == 3);
    }
}

TEST_CASE("meta/const_method") {
    namespace meta = meta_hpp;

    meta::method_ void_f_void_method_("void_f_void", &clazz::const_void_f_void);
    meta::method_ void_f_int_method_("void_f_int", &clazz::const_void_f_int);
    meta::method_ void_f_int2_method_("void_f_int2", &clazz::const_void_f_int2);

    meta::method_ int_f_void_method_("int_f_void", &clazz::const_int_f_void);
    meta::method_ int_f_int_method_("int_f_int", &clazz::const_int_f_int);
    meta::method_ int_f_int2_method_("int_f_int2", &clazz::const_int_f_int2);

    const meta::method_info& void_f_void_info = void_f_void_method_;
    const meta::method_info& void_f_int_info = void_f_int_method_;
    const meta::method_info& void_f_int2_info = void_f_int2_method_;

    const meta::method_info& int_f_void_info = int_f_void_method_;
    const meta::method_info& int_f_int_info = int_f_int_method_;
    const meta::method_info& int_f_int2_info = int_f_int2_method_;

    SUBCASE("instance_type") {
        CHECK(void_f_void_info.instance_type() == meta::get_family_id<clazz>());
        CHECK(int_f_void_info.instance_type() == meta::get_family_id<clazz>());
    }

    SUBCASE("return_type") {
        CHECK_FALSE(void_f_void_info.return_type());
        CHECK(int_f_void_info.return_type() == meta::get_family_id<int>());
    }

    SUBCASE("argument_types") {
        CHECK_FALSE(void_f_void_info.argument_type(0u));

        CHECK(int_f_int_info.argument_type(0u) == meta::get_family_id<int>());
        CHECK_FALSE(int_f_int_info.argument_type(1u));

        CHECK(int_f_int2_info.argument_type(0u) == meta::get_family_id<int>());
        CHECK(int_f_int2_info.argument_type(1u) == meta::get_family_id<int>());
        CHECK_FALSE(int_f_int2_info.argument_type(2u));
    }

    SUBCASE("another_instance") {
        const clazz2 instance;
        CHECK_THROWS_AS(void_f_void_info.invoke(instance), std::logic_error);
        CHECK_THROWS_AS(void_f_int_info.invoke(instance, 1), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1, 2), std::logic_error);
    }

    SUBCASE("void_return") {
        clazz instance;

        CHECK_NOTHROW(void_f_void_info.invoke(instance));
        CHECK_THROWS_AS(void_f_void_info.invoke(instance, 1), std::logic_error);

        CHECK_THROWS_AS(void_f_int_info.invoke(instance), std::logic_error);
        CHECK_NOTHROW(void_f_int_info.invoke(instance, 1));
        CHECK_THROWS_AS(void_f_int_info.invoke(instance, 1.f), std::logic_error);
        CHECK_THROWS_AS(void_f_int_info.invoke(instance, 1, 2), std::logic_error);

        CHECK_THROWS_AS(void_f_int2_info.invoke(instance), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1), std::logic_error);
        CHECK_NOTHROW(void_f_int2_info.invoke(instance, 1, 2));
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1.f, 2), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1, 2.f), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(instance, 1, 2, 3), std::logic_error);

        const clazz& cinstance = instance;
        CHECK_NOTHROW(void_f_void_info.invoke(cinstance));
        CHECK_NOTHROW(void_f_int_info.invoke(cinstance, 1));
        CHECK_NOTHROW(void_f_int2_info.invoke(cinstance, 1, 2));
    }

    SUBCASE("int_return") {
        clazz instance;

        CHECK(int_f_void_info.invoke(instance)->cast<int>() == 1);
        CHECK_THROWS_AS(int_f_void_info.invoke(instance, 1), std::logic_error);

        CHECK_THROWS_AS(int_f_int_info.invoke(instance), std::logic_error);
        CHECK(int_f_int_info.invoke(instance, 1)->cast<int>() == 1);
        CHECK_THROWS_AS(int_f_int_info.invoke(instance, 1.f), std::logic_error);
        CHECK_THROWS_AS(int_f_int_info.invoke(instance, 1, 2), std::logic_error);

        CHECK_THROWS_AS(int_f_int2_info.invoke(instance), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1), std::logic_error);
        CHECK(int_f_int2_info.invoke(instance, 1, 2)->cast<int>() == 3);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1.f, 2), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1, 2.f), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(instance, 1, 2, 3), std::logic_error);

        const clazz& cinstance = instance;
        CHECK(int_f_void_info.invoke(cinstance)->cast<int>() == 1);
        CHECK(int_f_int_info.invoke(cinstance, 1)->cast<int>() == 1);
        CHECK(int_f_int2_info.invoke(cinstance, 1, 2)->cast<int>() == 3);
    }

    SUBCASE("int_return_by_value") {
        meta::value instance{clazz{}};
        CHECK(int_f_void_info.invoke(instance)->cast<int>() == 1);
        CHECK(int_f_int_info.invoke(instance, 1)->cast<int>() == 1);
        CHECK(int_f_int2_info.invoke(instance, 1, 2)->cast<int>() == 3);
    }
}
