/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_value.hpp>
#include "doctest/doctest.hpp"

#include <string>
#include <vector>

namespace
{
    class clazz {};
    class clazz2 {};
}

TEST_CASE("meta/value") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    CHECK(meta::value{true}.to_bool() == true);
    CHECK(meta::value{1}.to_int() == 1);
    CHECK(meta::value{1u}.to_uint() == 1u);
    CHECK(meta::value{1.f}.to_float() == 1.f);
    CHECK(meta::value{1.0}.to_double() == 1.0);
    CHECK(meta::value{"meta"s}.to_string() == "meta");

    CHECK(meta::value{std::in_place_type<std::int8_t>, std::int8_t{1}}.to_int8() == 1);
    CHECK(meta::value{std::in_place_type<std::int16_t>, std::int16_t{1}}.to_int16() == 1);
    CHECK(meta::value{std::in_place_type<std::int32_t>, std::int32_t{1}}.to_int32() == 1);
    CHECK(meta::value{std::in_place_type<std::int64_t>, std::int64_t{1}}.to_int64() == 1);
    CHECK(meta::value{std::in_place_type<std::ptrdiff_t>, std::ptrdiff_t{1}}.to_ptrdiff_t() == 1);
    CHECK(meta::value{std::in_place_type<std::intptr_t>, std::intptr_t{1}}.to_intptr_t() == 1);

    CHECK(meta::value{std::in_place_type<std::uint8_t>, std::uint8_t{1}}.to_uint8() == 1u);
    CHECK(meta::value{std::in_place_type<std::uint16_t>, std::uint16_t{1}}.to_uint16() == 1u);
    CHECK(meta::value{std::in_place_type<std::uint32_t>, std::uint32_t{1}}.to_uint32() == 1u);
    CHECK(meta::value{std::in_place_type<std::uint64_t>, std::uint64_t{1}}.to_uint64() == 1u);
    CHECK(meta::value{std::in_place_type<std::size_t>, std::size_t{1}}.to_size_t() == 1u);
    CHECK(meta::value{std::in_place_type<std::uintptr_t>, std::uintptr_t{1}}.to_uintptr_t() == 1u);
}

TEST_CASE("meta/value/fid") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    CHECK(meta::value{clazz{}}.fid() == meta::get_type_family_id<clazz>());
    CHECK(meta::value{clazz2{}}.fid() == meta::get_type_family_id<clazz2>());
    CHECK(meta::value{clazz{}}.fid() != meta::value{clazz2{}}.fid());

    {
        meta::value v = clazz{};
        CHECK(v.fid() == meta::get_type_family_id<clazz>());
        v = clazz2{};
        CHECK(v.fid() == meta::get_type_family_id<clazz2>());
    }

    CHECK(meta::value{std::in_place_type<int>, 1}.fid() == meta::get_type_family_id<int>());
    CHECK(meta::value{std::in_place_type<std::vector<int>>, {1,2,3,4}}.fid() == meta::get_type_family_id<std::vector<int>>());
}

TEST_CASE("meta/value/swap") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::value v1{"hello"s};
    meta::value v2{42};
    meta::swap(v1, v2);

    CHECK(v1.fid() == meta::get_type_family_id<int>());
    CHECK(v1.to_int() == 42);

    CHECK(v2.fid() == meta::get_type_family_id<std::string>());
    CHECK(v2.to_string() == "hello");
}
