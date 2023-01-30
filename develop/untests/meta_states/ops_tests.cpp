/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_states/ops") {
    namespace meta = meta_hpp;

    SUBCASE("operator<") {
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
