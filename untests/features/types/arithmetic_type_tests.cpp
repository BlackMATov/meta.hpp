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

TEST_CASE("features/types/arithmetic") {
    SUBCASE("int") {
        using type = int;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<arithmetic_type>());

        const arithmetic_type at = type_db::get<type>().as<arithmetic_type>();

        CHECK_FALSE(at.raw_type());
        CHECK(at.size() == sizeof(type));

        CHECK(at.flags() == (
            arithmetic_flags::is_signed |
            arithmetic_flags::is_integral));

        CHECK_FALSE(at.is_const());
        CHECK(at.is_signed());
        CHECK_FALSE(at.is_unsigned());
        CHECK(at.is_integral());
        CHECK_FALSE(at.is_floating_point());
    }

    SUBCASE("const float") {
        using type = const float;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<arithmetic_type>());

        const arithmetic_type at = type_db::get<type>().as<arithmetic_type>();

        CHECK(at.raw_type() == type_db::get<float>());
        CHECK(at.size() == sizeof(type));

        CHECK(at.flags() == (
            arithmetic_flags::is_const |
            arithmetic_flags::is_signed |
            arithmetic_flags::is_floating_point));

        CHECK(at.is_const());
        CHECK(at.is_signed());
        CHECK_FALSE(at.is_unsigned());
        CHECK_FALSE(at.is_integral());
        CHECK(at.is_floating_point());
    }

    SUBCASE("const unsigned") {
        using type = const unsigned;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<arithmetic_type>());

        const arithmetic_type at = type_db::get<type>().as<arithmetic_type>();

        CHECK(at.raw_type() == type_db::get<unsigned>());
        CHECK(at.size() == sizeof(type));

        CHECK(at.flags() == (
            arithmetic_flags::is_const |
            arithmetic_flags::is_unsigned |
            arithmetic_flags::is_integral));

        CHECK(at.is_const());
        CHECK_FALSE(at.is_signed());
        CHECK(at.is_unsigned());
        CHECK(at.is_integral());
        CHECK_FALSE(at.is_floating_point());
    }
}
