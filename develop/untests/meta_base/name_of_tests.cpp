/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace meta_tests_ns
{
    struct A {
        int member{42};
        int add(int) {return 42;}
        static int sub(int) { return 42;}
    };

    enum class E {};
}

TEST_CASE("meta/meta_base/name_of") {
    namespace meta = meta_hpp;
    using meta::detail::name_of;

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_CLANG_COMPILER_ID
    CHECK(name_of<meta_tests_ns::E>() == "meta_tests_ns::E");

    CHECK(name_of<meta_tests_ns::A>() == "meta_tests_ns::A");
    CHECK(name_of<meta_tests_ns::A[]>() == "meta_tests_ns::A[]");
    CHECK(name_of<meta_tests_ns::A*>() == "meta_tests_ns::A *");
    CHECK(name_of<meta_tests_ns::A&>() == "meta_tests_ns::A &");
    CHECK(name_of<const meta_tests_ns::A&>() == "const meta_tests_ns::A &");

    CHECK(name_of<decltype(&meta_tests_ns::A::add)>() == "int (meta_tests_ns::A::*)(int)");
    CHECK(name_of<decltype(&meta_tests_ns::A::sub)>() == "int (*)(int)");
    CHECK(name_of<decltype(&meta_tests_ns::A::member)>() == "int meta_tests_ns::A::*");

    CHECK(name_of<int>() == "int");
    CHECK(name_of<float>() == "float");

    CHECK(name_of<void>() == "void");
    CHECK(name_of<const void>() == "const void");
#endif

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_GCC_COMPILER_ID
    CHECK(name_of<meta_tests_ns::E>() == "meta_tests_ns::E");

    CHECK(name_of<meta_tests_ns::A>() == "meta_tests_ns::A");
    CHECK(name_of<meta_tests_ns::A[]>() == "meta_tests_ns::A []");
    CHECK(name_of<meta_tests_ns::A*>() == "meta_tests_ns::A*");
    CHECK(name_of<meta_tests_ns::A&>() == "meta_tests_ns::A&");
    CHECK(name_of<const meta_tests_ns::A&>() == "const meta_tests_ns::A&");

    CHECK(name_of<decltype(&meta_tests_ns::A::add)>() == "int (meta_tests_ns::A::*)(int)");
    CHECK(name_of<decltype(&meta_tests_ns::A::sub)>() == "int (*)(int)");
    CHECK(name_of<decltype(&meta_tests_ns::A::member)>() == "int meta_tests_ns::A::*");

    CHECK(name_of<int>() == "int");
    CHECK(name_of<float>() == "float");

    CHECK(name_of<void>() == "void");
    CHECK(name_of<const void>() == "const void");
#endif

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_MSVC_COMPILER_ID
    CHECK(name_of<meta_tests_ns::E>() == "enum meta_tests_ns::E");

    CHECK(name_of<meta_tests_ns::A>() == "struct meta_tests_ns::A");
    CHECK(name_of<meta_tests_ns::A[]>() == "struct meta_tests_ns::A [0]");
    CHECK(name_of<meta_tests_ns::A*>() == "struct meta_tests_ns::A *");
    CHECK(name_of<meta_tests_ns::A&>() == "struct meta_tests_ns::A &");
    CHECK(name_of<const meta_tests_ns::A&>() == "struct meta_tests_ns::A const &");

    CHECK(name_of<decltype(&meta_tests_ns::A::add)>() == "int (__cdecl meta_tests_ns::A::*)(int)");
    CHECK(name_of<decltype(&meta_tests_ns::A::sub)>() == "int (__cdecl*)(int)");
    CHECK(name_of<decltype(&meta_tests_ns::A::member)>() == "int meta_tests_ns::A::*");

    CHECK(name_of<int>() == "int");
    CHECK(name_of<float>() == "float");

    CHECK(name_of<void>() == "void");
    CHECK(name_of<const void>() == "void const");
#endif
}
