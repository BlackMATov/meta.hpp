/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_types/array_type") {
    namespace meta = meta_hpp;

    SUBCASE("") {
        const meta::array_type type;
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());
    }

    SUBCASE("traits") {
        using meta::detail::array_traits;

        static_assert(array_traits<int[]>::extent == 0);
        static_assert(array_traits<const int[]>::extent == 0);
        static_assert(array_traits<const volatile int[]>::extent == 0);

        static_assert(array_traits<int[42]>::extent == 42);
        static_assert(array_traits<const int[42]>::extent == 42);
        static_assert(array_traits<const volatile int[42]>::extent == 42);

        static_assert(!array_traits<int[42]>::is_readonly);
        static_assert(array_traits<const int[42]>::is_readonly);
        static_assert(array_traits<const volatile int[42]>::is_readonly);

        static_assert(!array_traits<int[42]>::is_volatile);
        static_assert(!array_traits<const int[42]>::is_volatile);
        static_assert(array_traits<const volatile int[42]>::is_volatile);

        static_assert(std::is_same_v<array_traits<int[42]>::data_type, int>);
        static_assert(std::is_same_v<array_traits<const int[42]>::data_type, int>);
        static_assert(std::is_same_v<array_traits<const volatile int[42]>::data_type, int>);

        static_assert(std::is_same_v<array_traits<int[42]>::cv_data_type, int>);
        static_assert(std::is_same_v<array_traits<const int[42]>::cv_data_type, const int>);
        static_assert(std::is_same_v<array_traits<const volatile int[42]>::cv_data_type, const volatile int>);
    }

    SUBCASE("int[]") {
        const meta::array_type type = meta::resolve_type<int[]>();
        REQUIRE(type);

        CHECK(type.get_flags() == (meta::array_flags::is_unbounded));

        CHECK(type.get_extent() == 0);
        CHECK(type.get_data_type() == meta::resolve_type<int>());
    }

    SUBCASE("const unsigned[42][21]") {
        const meta::array_type type = meta::resolve_type<const unsigned[42][21]>();
        REQUIRE(type);

        CHECK(type.get_flags() == (meta::array_flags::is_bounded));

        CHECK(type.get_extent() == 42);
        CHECK(type.get_data_type() == meta::resolve_type<const unsigned[21]>());
    }
}
