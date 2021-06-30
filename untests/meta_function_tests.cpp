/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_function.hpp>
#include "doctest/doctest.hpp"

namespace
{
    void void_f_void() noexcept {}
    void void_f_int(int a) { (void)a; }
    void void_f_int2(int a, int b) { (void)a; (void)b; }

    int int_f_void() noexcept { return 1; }
    int int_f_int(int a) { return a; }
    int int_f_int2(int a, int b) { return a + b; }
}

TEST_CASE("meta/function") {
    namespace meta = meta_hpp;

    meta::function_<&void_f_void> void_f_void_function_("void_f_void");
    meta::function_<&void_f_int> void_f_int_function_("void_f_int");
    meta::function_<&void_f_int2> void_f_int2_function_("void_f_int2");

    meta::function_<&int_f_void> int_f_void_function_("int_f_void");
    meta::function_<&int_f_int> int_f_int_function_("int_f_int");
    meta::function_<&int_f_int2> int_f_int2_function_("int_f_int2");

    const meta::function_info& void_f_void_info = void_f_void_function_;
    const meta::function_info& void_f_int_info = void_f_int_function_;
    const meta::function_info& void_f_int2_info = void_f_int2_function_;

    const meta::function_info& int_f_void_info = int_f_void_function_;
    const meta::function_info& int_f_int_info = int_f_int_function_;
    const meta::function_info& int_f_int2_info = int_f_int2_function_;

    SUBCASE("void_return") {
        CHECK_NOTHROW(void_f_void_info.invoke());
        CHECK_THROWS_AS(void_f_void_info.invoke(1), std::logic_error);

        CHECK_THROWS_AS(void_f_int_info.invoke(), std::logic_error);
        CHECK_NOTHROW(void_f_int_info.invoke(1));
        CHECK_THROWS_AS(void_f_int_info.invoke(1.f), std::logic_error);
        CHECK_THROWS_AS(void_f_int_info.invoke(1, 2), std::logic_error);

        CHECK_THROWS_AS(void_f_int2_info.invoke(), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(1), std::logic_error);
        CHECK_NOTHROW(void_f_int2_info.invoke(1, 2));
        CHECK_THROWS_AS(void_f_int2_info.invoke(1.f, 2), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(1, 2.f), std::logic_error);
        CHECK_THROWS_AS(void_f_int2_info.invoke(1, 2, 3), std::logic_error);
    }

    SUBCASE("int_return") {
        CHECK(int_f_void_info.invoke().cast<int>() == 1);
        CHECK_THROWS_AS(int_f_void_info.invoke(1), std::logic_error);

        CHECK_THROWS_AS(int_f_int_info.invoke(), std::logic_error);
        CHECK(int_f_int_info.invoke(1).cast<int>() == 1);
        CHECK_THROWS_AS(int_f_int_info.invoke(1.f), std::logic_error);
        CHECK_THROWS_AS(int_f_int_info.invoke(1, 2), std::logic_error);

        CHECK_THROWS_AS(int_f_int2_info.invoke(), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(1), std::logic_error);
        CHECK(int_f_int2_info.invoke(1, 2).cast<int>() == 3);
        CHECK_THROWS_AS(int_f_int2_info.invoke(1.f, 2), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(1, 2.f), std::logic_error);
        CHECK_THROWS_AS(int_f_int2_info.invoke(1, 2, 3), std::logic_error);
    }
}
