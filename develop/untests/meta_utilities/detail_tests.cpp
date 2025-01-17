/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_utilities/detail") {
    namespace meta = meta_hpp;

    SUBCASE("cvref_traits") {
        static_assert(!meta::detail::cvref_traits<int>::is_lvalue);
        static_assert(!meta::detail::cvref_traits<int>::is_rvalue);
        static_assert(!meta::detail::cvref_traits<int>::is_const);
        static_assert(!meta::detail::cvref_traits<int>::is_volatile);

        static_assert(!meta::detail::cvref_traits<const int>::is_lvalue);
        static_assert(!meta::detail::cvref_traits<const int>::is_rvalue);
        static_assert(meta::detail::cvref_traits<const int>::is_const);
        static_assert(!meta::detail::cvref_traits<const int>::is_volatile);

        static_assert(!meta::detail::cvref_traits<const volatile int>::is_lvalue);
        static_assert(!meta::detail::cvref_traits<const volatile int>::is_rvalue);
        static_assert(meta::detail::cvref_traits<const volatile int>::is_const);
        static_assert(meta::detail::cvref_traits<const volatile int>::is_volatile);

        static_assert(meta::detail::cvref_traits<int&>::is_lvalue);
        static_assert(!meta::detail::cvref_traits<int&>::is_rvalue);
        static_assert(!meta::detail::cvref_traits<int&>::is_const);
        static_assert(!meta::detail::cvref_traits<int&>::is_volatile);

        static_assert(meta::detail::cvref_traits<const int&>::is_lvalue);
        static_assert(!meta::detail::cvref_traits<const int&>::is_rvalue);
        static_assert(meta::detail::cvref_traits<const int&>::is_const);
        static_assert(!meta::detail::cvref_traits<const int&>::is_volatile);

        static_assert(meta::detail::cvref_traits<const volatile int&>::is_lvalue);
        static_assert(!meta::detail::cvref_traits<const volatile int&>::is_rvalue);
        static_assert(meta::detail::cvref_traits<const volatile int&>::is_const);
        static_assert(meta::detail::cvref_traits<const volatile int&>::is_volatile);

        static_assert(!meta::detail::cvref_traits<int&&>::is_lvalue);
        static_assert(meta::detail::cvref_traits<int&&>::is_rvalue);
        static_assert(!meta::detail::cvref_traits<int&&>::is_const);
        static_assert(!meta::detail::cvref_traits<int&&>::is_volatile);

        static_assert(!meta::detail::cvref_traits<const int&&>::is_lvalue);
        static_assert(meta::detail::cvref_traits<const int&&>::is_rvalue);
        static_assert(meta::detail::cvref_traits<const int&&>::is_const);
        static_assert(!meta::detail::cvref_traits<const int&&>::is_volatile);

        static_assert(!meta::detail::cvref_traits<const volatile int&&>::is_lvalue);
        static_assert(meta::detail::cvref_traits<const volatile int&&>::is_rvalue);
        static_assert(meta::detail::cvref_traits<const volatile int&&>::is_const);
        static_assert(meta::detail::cvref_traits<const volatile int&&>::is_volatile);
    }

    SUBCASE("cvref_traits::copy_to") {
        static_assert(std::is_same_v<meta::detail::cvref_traits<float>::copy_to<int>, int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const float>::copy_to<int>, const int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const volatile float>::copy_to<int>, const volatile int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<float&>::copy_to<int>, int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const float&>::copy_to<int>, const int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<volatile float&>::copy_to<int>, volatile int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const volatile float&&>::copy_to<int>, const volatile int&&>);

        static_assert(std::is_same_v<meta::detail::cvref_traits<float>::copy_to<int&>, int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const float>::copy_to<int&>, const int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const volatile float>::copy_to<int&>, const volatile int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<float&>::copy_to<int&>, int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const float&>::copy_to<int&>, const int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<volatile float&>::copy_to<int&>, volatile int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const volatile float&&>::copy_to<int&>, const volatile int&&>);

        static_assert(std::is_same_v<meta::detail::cvref_traits<float>::copy_to<const volatile int&&>, int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const float>::copy_to<const volatile int&&>, const int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const volatile float>::copy_to<const volatile int&&>, const volatile int>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<float&>::copy_to<const volatile int&&>, int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const float&>::copy_to<const volatile int&&>, const int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<volatile float&>::copy_to<const volatile int&&>, volatile int&>);
        static_assert(std::is_same_v<meta::detail::cvref_traits<const volatile float&&>::copy_to<const volatile int&&>, const volatile int&&>);
    }
}
