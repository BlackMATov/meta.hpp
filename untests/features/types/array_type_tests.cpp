/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_types_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

TEST_CASE("features/types/array") {
    SUBCASE("int[]") {
        using type = int[];

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<array_type>());

        const array_type at = type_db::get<type>().as<array_type>();
        CHECK(at.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(at.data_type().id() == type_db::get<int>().id());
        CHECK(at.extent() == 0);
        CHECK_FALSE(at.is_bounded());
        CHECK(at.is_unbounded());
    }

    SUBCASE("const unsigned[42]") {
        using type = const unsigned[42];

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<array_type>());

        const array_type at = type_db::get<type>().as<array_type>();
        CHECK(at.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(at.data_type().id() == type_db::get<const unsigned>().id());
        CHECK(at.extent() == 42);
        CHECK(at.is_bounded());
        CHECK_FALSE(at.is_unbounded());
    }
}
