/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A1 { std::string a1{"a1"}; };
    struct B1 : A1 { std::string b1{"b1"}; META_HPP_ENABLE_BASE_INFO(A1) };
    struct C1 : A1 { std::string c1{"c1"}; META_HPP_ENABLE_BASE_INFO(A1) };
    struct D1 : B1, C1 { std::string d1{"d1"}; META_HPP_ENABLE_BASE_INFO(B1, C1) };

    struct A2 { std::string a2{"a2"}; };
    struct B2 : virtual A2 { std::string b2{"b2"}; META_HPP_ENABLE_BASE_INFO(A2) };
    struct C2 : virtual A2 { std::string c2{"c2"}; META_HPP_ENABLE_BASE_INFO(A2) };
    struct D2 : B2, C2 { std::string d2{"d2"}; META_HPP_ENABLE_BASE_INFO(B2, C2) };

    struct A3 { std::string a3{"a3"}; };
    struct B3 : virtual A3 { std::string b3{"b3"}; META_HPP_ENABLE_BASE_INFO(A3) };
    struct C3 : A3 { std::string c3{"c3"}; META_HPP_ENABLE_BASE_INFO(A3) };
    struct D3 : B3, C3 { std::string d3{"d3"}; META_HPP_ENABLE_BASE_INFO(B3, C3) };
}

TEST_CASE("meta/meta_features/ambiguous") {
    namespace meta = meta_hpp;

    {
        // A1 < B1
        //        < D1
        // A1 < C1

        static_assert(std::is_invocable_v<void(const A1&), A1&&>);
        static_assert(std::is_invocable_v<void(const A1&), B1&&>);
        static_assert(std::is_invocable_v<void(const A1&), C1&&>);
        static_assert(!std::is_invocable_v<void(const A1&), D1&&>);

        CHECK(meta::is_invocable_with(+[](const A1&){}, A1{}));
        CHECK(meta::is_invocable_with(+[](const A1&){}, B1{}));
        CHECK(meta::is_invocable_with(+[](const A1&){}, C1{}));
        CHECK_FALSE(meta::is_invocable_with(+[](const A1&){}, D1{}));
    }

    {
        // A2 <= B2
        //         < D2
        // A2 <= C2

        static_assert(std::is_invocable_v<void(const A2&), A2&&>);
        static_assert(std::is_invocable_v<void(const A2&), B2&&>);
        static_assert(std::is_invocable_v<void(const A2&), C2&&>);
        static_assert(std::is_invocable_v<void(const A2&), D2&&>);

        CHECK(meta::is_invocable_with(+[](const A2&){}, A2{}));
        CHECK(meta::is_invocable_with(+[](const A2&){}, B2{}));
        CHECK(meta::is_invocable_with(+[](const A2&){}, C2{}));
        CHECK(meta::is_invocable_with(+[](const A2&){}, D2{}));
    }

    {
        // A3 <= B3
        //         < D3
        // A3 <  C3

        static_assert(std::is_invocable_v<void(const A3&), A3&&>);
        static_assert(std::is_invocable_v<void(const A3&), B3&&>);
        static_assert(std::is_invocable_v<void(const A3&), C3&&>);
        static_assert(!std::is_invocable_v<void(const A3&), D3&&>);

        CHECK(meta::is_invocable_with(+[](const A3&){}, A3{}));
        CHECK(meta::is_invocable_with(+[](const A3&){}, B3{}));
        CHECK(meta::is_invocable_with(+[](const A3&){}, C3{}));
        CHECK_FALSE(meta::is_invocable_with(+[](const A3&){}, D3{}));
    }
}
