/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 {
        int x{};
        int y{};
    };
}

TEST_CASE("meta/meta_states/ops/_") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y);
}

TEST_CASE("meta/meta_states/ops") {
    namespace meta = meta_hpp;

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    const meta::member ivec2_x = ivec2_type.get_member("x");
    REQUIRE(ivec2_x);

    const meta::member ivec2_y = ivec2_type.get_member("y");
    REQUIRE(ivec2_y);

    SUBCASE("hash") {
        CHECK(std::hash<meta::member>{}(ivec2_x) == std::hash<meta::member>{}(ivec2_x));
        CHECK_FALSE(std::hash<meta::member>{}(ivec2_x) == std::hash<meta::member>{}({}));
        CHECK_FALSE(std::hash<meta::member>{}(ivec2_x) == std::hash<meta::member>{}(ivec2_y));
    }

    SUBCASE("operator<") {
        {
            CHECK(meta::member{} < ivec2_x);
            CHECK_FALSE(meta::member{} == ivec2_x);
            CHECK(meta::member{} < ivec2_x.get_index());
            CHECK_FALSE(meta::member{} == ivec2_x.get_index());

            CHECK_FALSE(ivec2_x < meta::member{});
            CHECK_FALSE(ivec2_x == meta::member{});
            CHECK_FALSE(ivec2_x.get_index() < meta::member{});
            CHECK_FALSE(ivec2_x.get_index() == meta::member{});

            CHECK(meta::member{} == meta::member{});
            CHECK_FALSE(meta::member{} < meta::member{});

            CHECK(ivec2_x == ivec2_x);
            CHECK(ivec2_x == ivec2_x.get_index());
            CHECK(ivec2_x.get_index() == ivec2_x);

            CHECK_FALSE(ivec2_x != ivec2_x);
            CHECK_FALSE(ivec2_x != ivec2_x.get_index());
            CHECK_FALSE(ivec2_x.get_index() != ivec2_x);

            CHECK(ivec2_x < ivec2_y);
            CHECK_FALSE(ivec2_y < ivec2_x);
            CHECK_FALSE(ivec2_x == ivec2_y);
        }

        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument>() < std::declval<meta::argument>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor>() < std::declval<meta::constructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor>() < std::declval<meta::destructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue>() < std::declval<meta::evalue>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function>() < std::declval<meta::function>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member>() < std::declval<meta::member>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method>() < std::declval<meta::method>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope>() < std::declval<meta::scope>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable>() < std::declval<meta::variable>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument>() < std::declval<meta::argument::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor>() < std::declval<meta::constructor::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor>() < std::declval<meta::destructor::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue>() < std::declval<meta::evalue::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function>() < std::declval<meta::function::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member>() < std::declval<meta::member::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method>() < std::declval<meta::method::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope>() < std::declval<meta::scope::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable>() < std::declval<meta::variable::index_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument::index_type>() < std::declval<meta::argument>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor::index_type>() < std::declval<meta::constructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor::index_type>() < std::declval<meta::destructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue::index_type>() < std::declval<meta::evalue>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function::index_type>() < std::declval<meta::function>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member::index_type>() < std::declval<meta::member>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method::index_type>() < std::declval<meta::method>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope::index_type>() < std::declval<meta::scope>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable::index_type>() < std::declval<meta::variable>())>);
        // NOLINTEND(*-redundant-expression)
    }

    SUBCASE("operator==") {
        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument>() == std::declval<meta::argument>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor>() == std::declval<meta::constructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor>() == std::declval<meta::destructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue>() == std::declval<meta::evalue>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function>() == std::declval<meta::function>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member>() == std::declval<meta::member>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method>() == std::declval<meta::method>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope>() == std::declval<meta::scope>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable>() == std::declval<meta::variable>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument>() == std::declval<meta::argument::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor>() == std::declval<meta::constructor::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor>() == std::declval<meta::destructor::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue>() == std::declval<meta::evalue::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function>() == std::declval<meta::function::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member>() == std::declval<meta::member::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method>() == std::declval<meta::method::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope>() == std::declval<meta::scope::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable>() == std::declval<meta::variable::index_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument::index_type>() == std::declval<meta::argument>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor::index_type>() == std::declval<meta::constructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor::index_type>() == std::declval<meta::destructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue::index_type>() == std::declval<meta::evalue>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function::index_type>() == std::declval<meta::function>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member::index_type>() == std::declval<meta::member>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method::index_type>() == std::declval<meta::method>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope::index_type>() == std::declval<meta::scope>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable::index_type>() == std::declval<meta::variable>())>);
        // NOLINTEND(*-redundant-expression)
    }

    SUBCASE("operator!=") {
        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument>() != std::declval<meta::argument>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor>() != std::declval<meta::constructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor>() != std::declval<meta::destructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue>() != std::declval<meta::evalue>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function>() != std::declval<meta::function>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member>() != std::declval<meta::member>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method>() != std::declval<meta::method>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope>() != std::declval<meta::scope>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable>() != std::declval<meta::variable>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument>() != std::declval<meta::argument::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor>() != std::declval<meta::constructor::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor>() != std::declval<meta::destructor::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue>() != std::declval<meta::evalue::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function>() != std::declval<meta::function::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member>() != std::declval<meta::member::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method>() != std::declval<meta::method::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope>() != std::declval<meta::scope::index_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable>() != std::declval<meta::variable::index_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::argument::index_type>() != std::declval<meta::argument>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor::index_type>() != std::declval<meta::constructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor::index_type>() != std::declval<meta::destructor>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::evalue::index_type>() != std::declval<meta::evalue>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function::index_type>() != std::declval<meta::function>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member::index_type>() != std::declval<meta::member>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method::index_type>() != std::declval<meta::method>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::scope::index_type>() != std::declval<meta::scope>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::variable::index_type>() != std::declval<meta::variable>())>);
        // NOLINTEND(*-redundant-expression)
    }
}
