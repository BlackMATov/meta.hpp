/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_base/type_list") {
    namespace meta = meta_hpp;
    using meta::detail::type_list;

    SUBCASE("type_list_at_t") {
        using meta::detail::type_list_at_t;

        static_assert(std::is_same_v<type_list_at_t<0, type_list<int, float>>, int>);
        static_assert(std::is_same_v<type_list_at_t<1, type_list<int, float>>, float>);
    }

    SUBCASE("type_list_arity_v") {
        using meta::detail::type_list_arity_v;

        static_assert(type_list_arity_v<type_list<>> == 0);
        static_assert(type_list_arity_v<type_list<int>> == 1);
        static_assert(type_list_arity_v<type_list<int, float>> == 2);
    }

    SUBCASE("type_list_count_of_v") {
        using meta::detail::type_list_count_of_v;

        static_assert(type_list_count_of_v<std::is_integral, type_list<>> == 0);
        static_assert(type_list_count_of_v<std::is_integral, type_list<float>> == 0);
        static_assert(type_list_count_of_v<std::is_integral, type_list<int, float>> == 1);
        static_assert(type_list_count_of_v<std::is_integral, type_list<int, float, int>> == 2);
        static_assert(type_list_count_of_v<std::is_integral, type_list<int, float, int, double, long>> == 3);
    }

    SUBCASE("type_list_first_of_t") {
        using meta::detail::type_list_first_of_t;

        static_assert(std::is_same_v<type_list_first_of_t<std::is_integral, double, type_list<int, float>>, int>);
        static_assert(std::is_same_v<type_list_first_of_t<std::is_integral, double, type_list<float, int>>, int>);
        static_assert(std::is_same_v<type_list_first_of_t<std::is_integral, double, type_list<int, float, long>>, int>);
        static_assert(std::is_same_v<type_list_first_of_t<std::is_integral, double, type_list<float, long, int>>, long>);

        static_assert(std::is_same_v<type_list_first_of_t<std::is_integral, long, type_list<float, double>>, long>);
    }

    SUBCASE("type_list_and_v") {
        using meta::detail::type_list_and_v;

        static_assert(type_list_and_v<std::is_integral, type_list<int, unsigned>>);
        static_assert(!type_list_and_v<std::is_integral, type_list<float, unsigned>>);
        static_assert(!type_list_and_v<std::is_integral, type_list<int, float>>);
        static_assert(!type_list_and_v<std::is_integral, type_list<float, double>>);
    }

    SUBCASE("type_list_or_v") {
        using meta::detail::type_list_or_v;

        static_assert(type_list_or_v<std::is_integral, type_list<int, unsigned>>);
        static_assert(type_list_or_v<std::is_integral, type_list<float, unsigned>>);
        static_assert(type_list_or_v<std::is_integral, type_list<int, float>>);
        static_assert(!type_list_or_v<std::is_integral, type_list<float, double>>);
    }
}
