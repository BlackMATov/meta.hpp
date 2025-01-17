/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct clazz_t {
        int int_member{};
        int int_method() { return 0; }
        static int int_function() { return 0; }
    };
}

TEST_CASE("meta/meta_issues/random/7") {
    namespace meta = meta_hpp;

    SUBCASE("member_pointer") {
        using r = int clazz_t::*;
        using w = float clazz_t::*;

        meta::uvalue v{&clazz_t::int_member};
        REQUIRE(v.get_type() == meta::resolve_type<r>());
        CHECK_FALSE(v.get_type() == meta::resolve_type<w>());

        REQUIRE(v.is<r>());
        CHECK_FALSE(v.is<w>());

        CHECK(v.as<r>() == &clazz_t::int_member);
        CHECK(v.try_as<r>() == &clazz_t::int_member);
        CHECK(v.try_as<w>() == nullptr);

        CHECK(std::as_const(v).as<r>() == &clazz_t::int_member);
        CHECK(std::as_const(v).try_as<r>() == &clazz_t::int_member);
        CHECK(std::as_const(v).try_as<w>() == nullptr);

        CHECK(v.copy().as<r>() == &clazz_t::int_member);
        CHECK(v.copy().try_as<r>() == &clazz_t::int_member);
        CHECK(v.copy().try_as<w>() == nullptr);
    }

    SUBCASE("method_pointer") {
        using r = int(clazz_t::*)();
        using w = float(clazz_t::*)();

        meta::uvalue v{&clazz_t::int_method};
        REQUIRE(v.get_type() == meta::resolve_type<r>());
        CHECK_FALSE(v.get_type() == meta::resolve_type<w>());

        REQUIRE(v.is<r>());
        CHECK_FALSE(v.is<w>());

        CHECK(v.as<r>() == &clazz_t::int_method);
        CHECK(v.try_as<r>() == &clazz_t::int_method);
        CHECK(v.try_as<w>() == nullptr);

        CHECK(std::as_const(v).as<r>() == &clazz_t::int_method);
        CHECK(std::as_const(v).try_as<r>() == &clazz_t::int_method);
        CHECK(std::as_const(v).try_as<w>() == nullptr);

        CHECK(v.copy().as<r>() == &clazz_t::int_method);
        CHECK(v.copy().try_as<r>() == &clazz_t::int_method);
        CHECK(v.copy().try_as<w>() == nullptr);
    }

    SUBCASE("function_pointer") {
        using r = int(*)();
        using w = float(*)();

        meta::uvalue v{&clazz_t::int_function};
        REQUIRE(v.get_type() == meta::resolve_type<r>());
        CHECK_FALSE(v.get_type() == meta::resolve_type<w>());

        REQUIRE(v.is<r>());
        CHECK_FALSE(v.is<w>());

        CHECK(v.as<r>() == &clazz_t::int_function);
        CHECK(v.try_as<r>() == &clazz_t::int_function);
        CHECK(v.try_as<w>() == nullptr);

        CHECK(std::as_const(v).as<r>() == &clazz_t::int_function);
        CHECK(std::as_const(v).try_as<r>() == &clazz_t::int_function);
        CHECK(std::as_const(v).try_as<w>() == nullptr);

        CHECK(v.copy().as<r>() == &clazz_t::int_function);
        CHECK(v.copy().try_as<r>() == &clazz_t::int_function);
        CHECK(v.copy().try_as<w>() == nullptr);
    }
}
