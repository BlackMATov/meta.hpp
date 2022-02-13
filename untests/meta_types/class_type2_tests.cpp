/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct clazz {
        struct internal_clazz {};
        enum class internal_enum {};
    };
}

TEST_CASE("meta/meta_types/class_type2") {
    namespace meta = meta_hpp;

    meta::class_<clazz>()
        .typedef_<clazz::internal_clazz>("internal_clazz")
        .typedef_<clazz::internal_enum>("internal_enum");

    const meta::class_type clazz_type = meta::resolve_type<clazz>();
    REQUIRE(clazz_type);

    CHECK(clazz_type.get_typedef("internal_clazz") == meta::resolve_type<clazz::internal_clazz>());
    CHECK(clazz_type.get_typedef("internal_enum") == meta::resolve_type<clazz::internal_enum>());

    CHECK(clazz_type.get_typedefs() == meta::typedef_map{
        {"internal_clazz", meta::resolve_type<clazz::internal_clazz>()},
        {"internal_enum", meta::resolve_type<clazz::internal_enum>()},
    });
}
