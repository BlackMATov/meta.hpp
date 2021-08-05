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

TEST_CASE("features/types/void") {
    SUBCASE("void") {
        using type = void;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<void_type>());

        const void_type vt = type_db::get<type>().as<void_type>();
        CHECK(vt.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(vt.raw_type().id() == type_db::get<type>().id());
        CHECK_FALSE(vt.is_const());
    }

    SUBCASE("const void") {
        using type = const void;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<void_type>());

        const void_type vt = type_db::get<type>().as<void_type>();
        CHECK(vt.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK(vt.raw_type().id() == type_db::get<void>().id());
        CHECK(vt.is_const());
    }
}