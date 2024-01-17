/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct move_only_clazz {
        int v{};

        move_only_clazz(int nv) : v{nv} {}

        move_only_clazz(move_only_clazz&&) = default;
        move_only_clazz(const move_only_clazz&) = delete;

        move_only_clazz& operator=(move_only_clazz&&) = default;
        move_only_clazz& operator=(const move_only_clazz&) = delete;

        static move_only_clazz static_ctor(int nv) {
            return move_only_clazz{nv};
        }

        move_only_clazz clone() {
            return move_only_clazz{v};
        }
    };
}

TEST_CASE("meta/meta_features/move_only/_") {
    namespace meta = meta_hpp;

    meta::class_<move_only_clazz>()
        .constructor_<int>()
        .function_("static_ctor", &move_only_clazz::static_ctor)
        .method_("clone", &move_only_clazz::clone);
}

TEST_CASE("meta/meta_features/move_only") {
    namespace meta = meta_hpp;

    const meta::class_type move_only_clazz_type = meta::resolve_type<move_only_clazz>();

    SUBCASE("constructor") {
        meta::uvalue v = move_only_clazz_type.create(42);
        REQUIRE(v.get_type() == move_only_clazz_type);
        CHECK(v.as<move_only_clazz>().v == 42);
    }

    SUBCASE("function") {
        meta::uvalue v = move_only_clazz_type.get_function("static_ctor").invoke(42);
        REQUIRE(v.get_type() == move_only_clazz_type);
        CHECK(v.as<move_only_clazz>().v == 42);
    }

    SUBCASE("method") {
        meta::uvalue v = move_only_clazz_type.get_method("clone").invoke(move_only_clazz{42});
        REQUIRE(v.get_type() == move_only_clazz_type);
        CHECK(v.as<move_only_clazz>().v == 42);
    }
}
