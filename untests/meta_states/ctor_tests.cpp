/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct clazz_1 {
        int i{};
        clazz_1(int i) : i{i} {}
    };

    struct clazz_2 {
        int i{};
        clazz_2(int i) : i{i} {}
    };

    struct clazz_3 {
        int i{};
        clazz_3(int i) : i{i} {}
    };
}

TEST_CASE("meta/meta_states/ctor") {
    namespace meta = meta_hpp;

    meta::class_<clazz_1>()
        .ctor_<int>(meta::ctor_policy::as_object{});

    meta::class_<clazz_2>()
        .ctor_<int>(meta::ctor_policy::as_raw_pointer{});

    meta::class_<clazz_3>()
        .ctor_<int>(meta::ctor_policy::as_shared_pointer{});

    SUBCASE("clazz_1") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_1>();
        REQUIRE(clazz_type);

        const meta::value v = clazz_type.create(10).value();
        CHECK(v.get_type() == meta::resolve_type<clazz_1>());
        CHECK(v.cast<clazz_1>().i == 10);
    }

    SUBCASE("clazz_2") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_2>();
        REQUIRE(clazz_type);

        const meta::value v = clazz_type.create(20).value();
        CHECK(v.get_type() == meta::resolve_type<clazz_2*>());
        CHECK(v.cast<clazz_2*>()->i == 20);

        //TODO: dtor!!!
    }

    SUBCASE("clazz_3") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_3>();
        REQUIRE(clazz_type);

        const meta::value v = clazz_type.create(30).value();
        CHECK(v.get_type() == meta::resolve_type<std::shared_ptr<clazz_3>>());
        CHECK(v.cast<std::shared_ptr<clazz_3>>()->i == 30);
    }
}
