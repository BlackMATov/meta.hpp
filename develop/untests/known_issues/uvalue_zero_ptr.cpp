/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/known_issues/uvalue_zero_ptr") {
    namespace meta = meta_hpp;

    // {
    //     void* my_null = static_cast<void*>(0);
    //     meta::uvalue v{my_null};
    //     CHECK(v.as<void*>() == my_null);
    //     CHECK(v.as<const void*>() == my_null);
    // }
    // {
    //     const void* my_null = static_cast<const void*>(0);
    //     meta::uvalue v{my_null};
    //     CHECK(v.as<const void*>() == my_null);
    // }
}
