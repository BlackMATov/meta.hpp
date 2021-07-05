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
        void void_f_int2(int a, int b) { (void)a; (void)b; }

        int int_f_void() noexcept { return 1; }
        int int_f_int(int a) { return a; }
        int int_f_int2(int a, int b) { return a + b; }

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

    meta::method_<&clazz::void_f_void> void_f_void_method_("void_f_void");
    meta::method_<&clazz::void_f_int> void_f_int_method_("void_f_int");
    meta::method_<&clazz::void_f_int2> void_f_int2_method_("void_f_int2");

    meta::method_<&clazz::int_f_void> int_f_void_method_("int_f_void");
    meta::method_<&clazz::int_f_int> int_f_int_method_("int_f_int");
    meta::method_<&clazz::int_f_int2> int_f_int2_method_("int_f_int2");

    const meta::method_info& void_f_void_info = void_f_void_method_;
    const meta::method_info& void_f_int_info = void_f_int_method_;
    const meta::method_info& void_f_int2_info = void_f_int2_method_;

    const meta::method_info& int_f_void_info = int_f_void_method_;
    const meta::method_info& int_f_int_info = int_f_int_method_;
    const meta::method_info& int_f_int2_info = int_f_int2_method_;

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

    meta::method_<&clazz::const_void_f_void> void_f_void_method_("void_f_void");
    meta::method_<&clazz::const_void_f_int> void_f_int_method_("void_f_int");
    meta::method_<&clazz::const_void_f_int2> void_f_int2_method_("void_f_int2");

    meta::method_<&clazz::const_int_f_void> int_f_void_method_("int_f_void");
    meta::method_<&clazz::const_int_f_int> int_f_int_method_("int_f_int");
    meta::method_<&clazz::const_int_f_int2> int_f_int2_method_("int_f_int2");

    const meta::method_info& void_f_void_info = void_f_void_method_;
    const meta::method_info& void_f_int_info = void_f_int_method_;
    const meta::method_info& void_f_int2_info = void_f_int2_method_;

    const meta::method_info& int_f_void_info = int_f_void_method_;
    const meta::method_info& int_f_int_info = int_f_int_method_;
    const meta::method_info& int_f_int2_info = int_f_int2_method_;

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
