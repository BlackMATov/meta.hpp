/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    template < typename Derived >
    struct base {};

    struct derived : base<derived> {
        META_HPP_ENABLE_BASE_INFO(base<derived>)
    };
}

TEST_CASE("meta/meta_issues/random/9") {
    namespace meta = meta_hpp;

    // meta::resolve_type<derived>();
}
