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
    struct B {};
}

TEST_CASE("meta/meta_issues/random/3") {
    namespace meta = meta_hpp;

    meta::detail::type_registry& registry{meta::detail::type_registry::instance()};

    meta::class_type a_type = registry.resolve_class_type<A>();
    meta::class_type b_type = registry.resolve_class_type<B>();

    CHECK(registry.get_type_by_id(a_type.get_id()) == a_type);
    CHECK(registry.get_type_by_id(b_type.get_id()) == b_type);
}
