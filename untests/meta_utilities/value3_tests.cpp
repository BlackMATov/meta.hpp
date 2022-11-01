/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct base0 {
        int i{21};
    };

    struct base1 : virtual base0 {
        int j{42};
    };

    struct base2 : virtual base0 {
        int k{84};
    };

    struct derived : base1, base2 {
        int l{168};
    };
}

TEST_CASE("meta/meta_utilities/value3") {
    namespace meta = meta_hpp;

    meta::class_<base0>();

    meta::class_<base1>()
        .base_<base0>();

    meta::class_<base2>()
        .base_<base0>();

    meta::class_<derived>()
        .base_<base1>()
        .base_<base2>();
}

TEST_CASE("meta/meta_utilities/value3/cast") {
    namespace meta = meta_hpp;

    SUBCASE("derived") {
        {
            meta::uvalue v{derived{}};
            CHECK(v.get_type() == meta::resolve_type<derived>());

            CHECK(v.cast<base0>().i == 21);
            CHECK(v.cast<base1>().j == 42);
            CHECK(v.cast<base2>().k == 84);
            CHECK(v.cast<derived>().l == 168);
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(v.get_type() == meta::resolve_type<derived>());

            CHECK(v.cast<base0>().i == 21);
            CHECK(v.cast<base1>().j == 42);
            CHECK(v.cast<base2>().k == 84);
            CHECK(v.cast<derived>().l == 168);
        }
    }
}

TEST_CASE("meta/meta_utilities/value3/try_cast") {
    namespace meta = meta_hpp;

    SUBCASE("derived") {
        {
            meta::uvalue v{derived{}};
            CHECK(v.get_type() == meta::resolve_type<derived>());

            CHECK((v.try_cast<base0>() && v.try_cast<base0>()->i == 21));
            CHECK((v.try_cast<base1>() && v.try_cast<base1>()->j == 42));
            CHECK((v.try_cast<base2>() && v.try_cast<base2>()->k == 84));
            CHECK((v.try_cast<derived>() && v.try_cast<derived>()->l == 168));
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(v.get_type() == meta::resolve_type<derived>());

            CHECK((v.try_cast<base0>() && v.try_cast<base0>()->i == 21));
            CHECK((v.try_cast<base1>() && v.try_cast<base1>()->j == 42));
            CHECK((v.try_cast<base2>() && v.try_cast<base2>()->k == 84));
            CHECK((v.try_cast<derived>() && v.try_cast<derived>()->l == 168));
        }
    }
}
