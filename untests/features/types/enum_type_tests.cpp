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
    enum class ecolor : unsigned {
        red = 0xFF0000,
        green = 0x00FF00,
        blue = 0x0000FF,
    };
}

TEST_CASE("features/types/enum") {
    SUBCASE("ecolor") {
        using type = ecolor;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<enum_type>());

        const enum_type et = type_db::get<type>().as<enum_type>();
        CHECK(et.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK_FALSE(et.raw_type());
        CHECK(et.underlying_type().id() == type_db::get<unsigned>().id());

        CHECK(et.flags() == (enum_flags{}));
        CHECK_FALSE(et.is_const());
    }

    SUBCASE("const ecolor") {
        using type = const ecolor;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<enum_type>());

        const enum_type et = type_db::get<type>().as<enum_type>();
        CHECK(et.id() == type_id{typename_arg<base_type::tag<type>>});

        REQUIRE(et.raw_type());
        CHECK(et.raw_type().id() == type_db::get<ecolor>().id());
        CHECK(et.underlying_type().id() == type_db::get<unsigned>().id());

        CHECK(et.flags() == (enum_flags::is_const));
        CHECK(et.is_const());
    }
}
