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
    struct clazz {
        int int_member{};
        const int const_int_member{10};
    };
}

TEST_CASE("features/types/member") {
    SUBCASE("clazz::int_member") {
        using type = decltype(&clazz::int_member);

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<member_type>());

        const member_type mt = type_db::get<type>().as<member_type>();
        CHECK(mt.class_type() == type_db::get<clazz>());
        CHECK(mt.value_type() == type_db::get<int>());
    }

    SUBCASE("clazz::const_int_member") {
        using type = decltype(&clazz::const_int_member);

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<member_type>());

        const member_type mt = type_db::get<type>().as<member_type>();
        CHECK(mt.class_type() == type_db::get<clazz>());
        CHECK(mt.value_type() == type_db::get<const int>());
    }
}
