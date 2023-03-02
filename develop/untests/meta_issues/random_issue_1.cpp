/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A {};
    struct B : A {};
    struct C : A {};
    struct D : B, C {};

    // A * <- B <- *
    //              D
    // A * <- C <- *
}

TEST_CASE("meta/meta_issues/random/1") {
    namespace meta = meta_hpp;

    meta::class_<B>().base_<A>();
    meta::class_<C>().base_<A>();
    meta::class_<D>().base_<B, C>();

    CHECK(meta::is_invocable_with(+[](const D&){ return true; }, D{}));
    CHECK(meta::is_invocable_with(+[](const C&){ return true; }, D{}));
    CHECK(meta::is_invocable_with(+[](const B&){ return true; }, D{}));
    CHECK_FALSE(meta::is_invocable_with(+[](const A&){ return true; }, D{}));

    CHECK(meta::invoke(+[](const D&){ return true; }, D{}));
    CHECK(meta::invoke(+[](const C&){ return true; }, D{}));
    CHECK(meta::invoke(+[](const B&){ return true; }, D{}));
    CHECK_THROWS(meta::invoke(+[](const A&){ return true; }, D{}));
}
