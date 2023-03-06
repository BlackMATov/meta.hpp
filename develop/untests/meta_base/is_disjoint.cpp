/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_base/is_disjoint") {
    namespace meta = meta_hpp;
    using meta::detail::is_disjoint;

    CHECK(is_disjoint(std::set<int>{}, std::set<int>{}));

    CHECK(is_disjoint(std::set<int>{1}, std::set<int>{}));
    CHECK(is_disjoint(std::set<int>{}, std::set<int>{1}));

    CHECK_FALSE(is_disjoint(std::set<int>{1}, std::set<int>{1}));

    CHECK_FALSE(is_disjoint(std::set<int>{1,2}, std::set<int>{1}));
    CHECK_FALSE(is_disjoint(std::set<int>{1}, std::set<int>{1,2}));

    CHECK_FALSE(is_disjoint(std::set<int>{1,2}, std::set<int>{1,3}));
    CHECK_FALSE(is_disjoint(std::set<int>{1,3}, std::set<int>{1,2}));

    CHECK(is_disjoint(std::set<int>{1,2}, std::set<int>{3,4}));
    CHECK(is_disjoint(std::set<int>{2,3}, std::set<int>{1,4,5}));
    CHECK(is_disjoint(std::set<int>{2,4}, std::set<int>{1,3,5}));
}
