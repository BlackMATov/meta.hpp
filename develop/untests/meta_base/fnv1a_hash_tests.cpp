/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_base/fnv1a_hash") {
    namespace meta = meta_hpp;
    using meta::detail::fnv1a_hash;

    SUBCASE("const T*, size_t") {
        constexpr const char* world = "world";
        constexpr const char* hello1 = "hello";
        constexpr const char* hello2 = "hello";
        static_assert(fnv1a_hash(world, 5) != fnv1a_hash(hello1, 5));
        static_assert(fnv1a_hash(hello1, 5) == fnv1a_hash(hello2, 5));
    }

    SUBCASE("const void*, size_t") {
        constexpr const void* world = "world";
        constexpr const void* hello1 = "hello";
        constexpr const void* hello2 = "hello";
        CHECK(fnv1a_hash(world, 5) != fnv1a_hash(hello1, 5));
        CHECK(fnv1a_hash(hello1, 5) == fnv1a_hash(hello2, 5));
    }
}
