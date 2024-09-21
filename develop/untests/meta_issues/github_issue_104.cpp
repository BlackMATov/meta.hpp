/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct clazz {};
}

TEST_CASE("meta/meta_issues/104") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::class_<clazz>()
        .destructor_(meta::metadata_()("hello", "world"s));

    CHECK(meta::resolve_type<clazz>().get_destructor().get_metadata().contains("hello"));

    meta::class_<clazz>();

    CHECK(meta::resolve_type<clazz>().get_destructor().get_metadata().contains("hello"));
}
