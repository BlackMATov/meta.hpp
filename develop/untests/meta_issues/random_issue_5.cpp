/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A1 {};
    struct B1 : A1 { META_HPP_ENABLE_BASE_INFO(A1) };
    struct C1 : A1 { META_HPP_ENABLE_BASE_INFO(A1) };
    struct D1 : B1, C1 { META_HPP_ENABLE_BASE_INFO(B1, C1) };
    struct E1 : D1 { META_HPP_ENABLE_BASE_INFO(D1) };

    // A1 < B1
    //        < D1 < E1
    // A1 < C1

    struct A2 {};
    struct B2 : virtual A2 { META_HPP_ENABLE_BASE_INFO(A2) };
    struct C2 : virtual A2 { META_HPP_ENABLE_BASE_INFO(A2) };
    struct D2 : B2, C2 { META_HPP_ENABLE_BASE_INFO(B2, C2) };
    struct E2 : D2 { META_HPP_ENABLE_BASE_INFO(D2) };

    // A2 <= B2
    //         < D2 < E2
    // A2 <= C2
}

TEST_CASE("meta/meta_issues/random/5") {
    namespace meta = meta_hpp;

    {
        CHECK(meta::is_invocable_with(+[](const A1&){}, B1{}));
        CHECK(meta::is_invocable_with(+[](const A1&){}, C1{}));
        CHECK(meta::is_invocable_with(+[](const B1&){}, D1{}));
        CHECK(meta::is_invocable_with(+[](const C1&){}, D1{}));
        CHECK(meta::is_invocable_with(+[](const B1&){}, E1{}));
        CHECK(meta::is_invocable_with(+[](const C1&){}, E1{}));
        CHECK(meta::is_invocable_with(+[](const D1&){}, E1{}));

        CHECK_FALSE(meta::is_invocable_with(+[](const A1&){}, D1{}));
        CHECK_FALSE(meta::is_invocable_with(+[](const A1&){}, E1{}));
    }

    {
        CHECK(meta::is_invocable_with(+[](const A2&){}, B2{}));
        CHECK(meta::is_invocable_with(+[](const A2&){}, C2{}));
        CHECK(meta::is_invocable_with(+[](const B2&){}, D2{}));
        CHECK(meta::is_invocable_with(+[](const C2&){}, D2{}));
        CHECK(meta::is_invocable_with(+[](const B2&){}, E2{}));
        CHECK(meta::is_invocable_with(+[](const C2&){}, E2{}));
        CHECK(meta::is_invocable_with(+[](const D2&){}, E2{}));

        CHECK(meta::is_invocable_with(+[](const A2&){}, D2{}));
        CHECK(meta::is_invocable_with(+[](const A2&){}, E2{}));
    }
}
