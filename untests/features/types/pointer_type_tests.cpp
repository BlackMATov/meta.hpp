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

namespace
{
    struct ivec2 {
        int x{};
        int y{};
    };
}

TEST_CASE("features/types/pointer") {
    SUBCASE("ivec2*") {
        using type = ivec2*;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<pointer_type>());

        const pointer_type pt = type_db::get<type>().as<pointer_type>();
        CHECK(pt.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(pt.data_type().id() == type_db::get<ivec2>().id());
        CHECK_FALSE(pt.is_const());
    }

    SUBCASE("const ivec2*") {
        using type = const ivec2*;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<pointer_type>());

        const pointer_type pt = type_db::get<type>().as<pointer_type>();
        CHECK(pt.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(pt.data_type().id() == type_db::get<const ivec2>().id());
        CHECK_FALSE(pt.is_const());
    }

    SUBCASE("ivec2* const") {
        using type = ivec2* const;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<pointer_type>());

        const pointer_type pt = type_db::get<type>().as<pointer_type>();
        CHECK(pt.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(pt.data_type().id() == type_db::get<ivec2>().id());
        CHECK(pt.is_const());
    }

    SUBCASE("const ivec2* const") {
        using type = const ivec2* const;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<pointer_type>());

        const pointer_type pt = type_db::get<type>().as<pointer_type>();
        CHECK(pt.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(pt.data_type().id() == type_db::get<const ivec2>().id());
        CHECK(pt.is_const());
    }
}
