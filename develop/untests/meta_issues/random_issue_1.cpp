/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A1 {};
    struct B1 : A1 { META_HPP_ENABLE_BASE_INFO(A1) };
    struct C1 : A1 { META_HPP_ENABLE_BASE_INFO(A1) };
    struct D1 : B1, C1 { META_HPP_ENABLE_BASE_INFO(B1, C1) };

    // A1 <- B1 <- *
    //              D1
    // A1 <- C1 <- *

    struct A2 {};
    struct B2 : virtual A2 { META_HPP_ENABLE_BASE_INFO(A2) };
    struct C2 : virtual B2 { META_HPP_ENABLE_BASE_INFO(B2) };
    struct D2 : virtual B2 { META_HPP_ENABLE_BASE_INFO(B2) };
    struct E2 : C2, D2 { META_HPP_ENABLE_BASE_INFO(C2, D2) };

    // A2 <= B2 <= C2 <- *
    //                    E2
    // A2 <= B2 <= D2 <- *

    struct A3 {};
    struct B3 : virtual A3 { META_HPP_ENABLE_BASE_INFO(A3) };
    struct C3 : A3 { META_HPP_ENABLE_BASE_INFO(A3) };
    struct D3 : B3, C3 { META_HPP_ENABLE_BASE_INFO(B3, C3) };

    // A3 <= B3 <- *
    //              D3
    // A3 <- C3 <- *
}

TEST_CASE("meta/meta_issues/random/1") {
    namespace meta = meta_hpp;

    {
        CHECK(meta::is_invocable_with(+[](const D1&){ return true; }, D1{}));
        CHECK(meta::is_invocable_with(+[](const C1&){ return true; }, D1{}));
        CHECK(meta::is_invocable_with(+[](const B1&){ return true; }, D1{}));
        CHECK_FALSE(meta::is_invocable_with(+[](const A1&){ return true; }, D1{}));

        CHECK(meta::try_invoke(+[](const D1&){ return true; }, D1{}));
        CHECK(meta::try_invoke(+[](const C1&){ return true; }, D1{}));
        CHECK(meta::try_invoke(+[](const B1&){ return true; }, D1{}));
        CHECK_FALSE(meta::try_invoke(+[](const A1&){ return true; }, D1{}));
    }

    {
        CHECK(meta::is_invocable_with(+[](const A2&){ return true; }, E2{}));
        CHECK(meta::is_invocable_with(+[](const B2&){ return true; }, E2{}));
        CHECK(meta::is_invocable_with(+[](const C2&){ return true; }, E2{}));
        CHECK(meta::is_invocable_with(+[](const D2&){ return true; }, E2{}));

        CHECK(meta::try_invoke(+[](const A2&){ return true; }, E2{}));
        CHECK(meta::try_invoke(+[](const B2&){ return true; }, E2{}));
        CHECK(meta::try_invoke(+[](const C2&){ return true; }, E2{}));
        CHECK(meta::try_invoke(+[](const D2&){ return true; }, E2{}));
    }

    {
        static_assert(std::is_invocable_v<void(A3*), A3*>);
        static_assert(std::is_invocable_v<void(A3*), B3*>);
        static_assert(std::is_invocable_v<void(A3*), C3*>);
        static_assert(!std::is_invocable_v<void(A3*), D3*>);

        CHECK(meta::is_invocable_with(+[](const A3&){ return true; }, A3{}));
        CHECK(meta::is_invocable_with(+[](const A3&){ return true; }, B3{}));
        CHECK(meta::is_invocable_with(+[](const A3&){ return true; }, C3{}));
        CHECK_FALSE(meta::is_invocable_with(+[](const A3&){ return true; }, D3{}));
    }
}
