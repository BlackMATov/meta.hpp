/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    class A{};
    class B{};
}

TEST_CASE("meta/meta_features/for_each_type") {
    namespace meta = meta_hpp;

    SUBCASE("any_type") {
        std::set<meta_hpp::any_type> all_types;
        meta::for_each_type([&all_types](const meta::any_type& type){
            all_types.insert(type);
        });
        CHECK_FALSE(all_types.contains(meta::resolve_type<A>()));

        meta::for_each_type([&all_types](const meta::any_type& type){
            all_types.insert(type);
        });
        CHECK(all_types.contains(meta::resolve_type<A>()));

    }

    SUBCASE("specific_type") {
        std::set<meta_hpp::array_type> all_types;

        meta::for_each_type<meta::array_type>([&all_types](const meta::array_type& type){
            all_types.insert(type);
        });
        CHECK_FALSE(all_types.contains(meta::resolve_type<A[]>()));

        meta::for_each_type<meta::array_type>([&all_types](const meta::array_type& type){
            all_types.insert(type);
        });
        CHECK(all_types.contains(meta::resolve_type<A[]>()));
    }
}
