/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct base_clazz {
    };

    struct derived_clazz final : base_clazz {
    };
}

TEST_CASE("meta/examples/class") {
    namespace meta = meta_hpp;

    meta::class_<derived_clazz>()
        .base_<base_clazz>();

    const meta::class_type base_clazz_type = meta::resolve_type<base_clazz>();
    REQUIRE(base_clazz_type);

    const meta::class_type derived_clazz_type = meta::resolve_type<derived_clazz>();
    REQUIRE(derived_clazz_type);

    CHECK(base_clazz_type.is_base_of<derived_clazz>());
    CHECK(derived_clazz_type.is_derived_from<base_clazz>());

    const meta::class_type int_vector_type = meta::resolve_type<std::vector<int>>();
    REQUIRE(int_vector_type);
    CHECK(int_vector_type.get_arity() == 2);
    CHECK(int_vector_type.get_argument_type(0) == meta::resolve_type<int>());
    CHECK(int_vector_type.get_argument_type(1) == meta::resolve_type<std::allocator<int>>());
}
