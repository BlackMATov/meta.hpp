/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_value.hpp>
#include "doctest/doctest.hpp"

namespace
{
}

TEST_CASE("meta/value") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    CHECK(meta::value{true}.to_bool() == true);
    CHECK(meta::value{1}.to_int() == 1);
    CHECK(meta::value{1.f}.to_float() == 1.f);
    CHECK(meta::value{1.0}.to_double() == 1.0);
    CHECK(meta::value{"meta"s}.to_string() == "meta");

    CHECK(meta::value{std::in_place_type<std::int8_t>, 1}.to_int8() == 1);
    CHECK(meta::value{std::in_place_type<std::int16_t>, 1}.to_int16() == 1);
    CHECK(meta::value{std::in_place_type<std::int32_t>, 1}.to_int32() == 1);
    CHECK(meta::value{std::in_place_type<std::int64_t>, 1}.to_int64() == 1);

    CHECK(meta::value{std::in_place_type<std::uint8_t>, 1}.to_uint8() == 1u);
    CHECK(meta::value{std::in_place_type<std::uint16_t>, 1}.to_uint16() == 1u);
    CHECK(meta::value{std::in_place_type<std::uint32_t>, 1}.to_uint32() == 1u);
    CHECK(meta::value{std::in_place_type<std::uint64_t>, 1}.to_uint64() == 1u);

    CHECK(meta::value{std::in_place_type<std::size_t>, 1}.to_size_t() == 1u);
    CHECK(meta::value{std::in_place_type<std::ptrdiff_t>, 1}.to_ptrdiff_t() == 1u);
}
