/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    int int_func() { return 0; }
}

TEST_CASE("meta/meta_issues/random/3") {
    namespace meta = meta_hpp;

    static_assert(std::is_same_v<decltype(meta::resolve_type<int()>()), meta_hpp::function_type>);
    static_assert(std::is_same_v<decltype(meta::resolve_type<int(*)()>()), meta_hpp::pointer_type>);
    static_assert(std::is_same_v<decltype(meta::resolve_type<int(&)()>()), meta_hpp::reference_type>);

    static_assert(std::is_same_v<decltype(meta::resolve_type(int_func)), meta_hpp::function_type>);
    static_assert(std::is_same_v<decltype(meta::resolve_type(&int_func)), meta_hpp::pointer_type>);

    CHECK(meta::resolve_type<int()>().get_kind() == meta::type_kind::function_);
    CHECK(meta::resolve_type<int(*)()>().get_kind() == meta::type_kind::pointer_);
    CHECK(meta::resolve_type<int(&)()>().get_kind() == meta::type_kind::reference_);

    CHECK(meta::resolve_type(int_func).get_kind() == meta::type_kind::function_);
    CHECK(meta::resolve_type(&int_func).get_kind() == meta::type_kind::pointer_);

    CHECK(meta::resolve_type<int(*)()>().get_data_type() == meta::resolve_type<int()>());
    CHECK(meta::resolve_type<int(&)()>().get_data_type() == meta::resolve_type<int()>());

    {
        meta::uvalue v{&int_func};
        CHECK_FALSE(v.has_deref_op());
        CHECK(v.get_type() == meta::resolve_type<int(*)()>());
        CHECK((v.as<decltype(&int_func)>() == &int_func));
    }
}
