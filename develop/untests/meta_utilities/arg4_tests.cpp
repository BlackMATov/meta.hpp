/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_utilities/arg4/cast") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;

    auto LV_PTR = []() -> int*& { static int v{42}; static int* p{&v}; return p; };
    auto CLV_PTR = []() -> int* const& { static int v{42}; static int* p{&v}; return p; };
    auto XV_PTR = []() -> int*&& { static int v{42}; static int* p{&v}; return std::move(p); };
    auto CXV_PTR = []() -> int* const&& { static int v{42}; static int* p{&v}; return std::move(p); };
    auto PRV_PTR = []() -> int* { static int v{42}; static int* p{&v}; return p; };

    auto LV_CPTR = []() -> const int*& { static int v{42}; static const int* p{&v}; return p; };
    auto CLV_CPTR = []() -> const int* const& { static int v{42}; static const int* p{&v}; return p; };
    auto XV_CPTR = []() -> const int*&& { static int v{42}; static const int* p{&v}; return std::move(p); };
    auto CXV_CPTR = []() -> const int* const&& { static int v{42}; static const int* p{&v}; return std::move(p); };
    auto PRV_CPTR = []() -> const int* { static int v{42}; static const int* p{&v}; return p; };

    SUBCASE("LV_PTR") {
        CHECK(uarg{LV_PTR()}.can_cast_to<int*>());
        CHECK(uarg{LV_PTR()}.can_cast_to<int* const>());
        CHECK(uarg{LV_PTR()}.can_cast_to<const int*>());
        CHECK(uarg{LV_PTR()}.can_cast_to<const int* const>());

        CHECK(*uarg{LV_PTR()}.cast<int*>() == 42);
        CHECK(*uarg{LV_PTR()}.cast<int* const>() == 42);
        CHECK(*uarg{LV_PTR()}.cast<const int*>() == 42);
        CHECK(*uarg{LV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CLV_PTR") {
        CHECK(uarg{CLV_PTR()}.can_cast_to<int*>());
        CHECK(uarg{CLV_PTR()}.can_cast_to<int* const>());
        CHECK(uarg{CLV_PTR()}.can_cast_to<const int*>());
        CHECK(uarg{CLV_PTR()}.can_cast_to<const int* const>());

        CHECK(*uarg{CLV_PTR()}.cast<int*>() == 42);
        CHECK(*uarg{CLV_PTR()}.cast<int* const>() == 42);
        CHECK(*uarg{CLV_PTR()}.cast<const int*>() == 42);
        CHECK(*uarg{CLV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("XV_PTR") {
        CHECK(uarg{XV_PTR()}.can_cast_to<int*>());
        CHECK(uarg{XV_PTR()}.can_cast_to<int* const>());
        CHECK(uarg{XV_PTR()}.can_cast_to<const int*>());
        CHECK(uarg{XV_PTR()}.can_cast_to<const int* const>());

        CHECK(*uarg{XV_PTR()}.cast<int*>() == 42);
        CHECK(*uarg{XV_PTR()}.cast<int* const>() == 42);
        CHECK(*uarg{XV_PTR()}.cast<const int*>() == 42);
        CHECK(*uarg{XV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CXV_PTR") {
        CHECK(uarg{CXV_PTR()}.can_cast_to<int*>());
        CHECK(uarg{CXV_PTR()}.can_cast_to<int* const>());
        CHECK(uarg{CXV_PTR()}.can_cast_to<const int*>());
        CHECK(uarg{CXV_PTR()}.can_cast_to<const int* const>());

        CHECK(*uarg{CXV_PTR()}.cast<int*>() == 42);
        CHECK(*uarg{CXV_PTR()}.cast<int* const>() == 42);
        CHECK(*uarg{CXV_PTR()}.cast<const int*>() == 42);
        CHECK(*uarg{CXV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("PRV_PTR") {
        CHECK(uarg{PRV_PTR()}.can_cast_to<int*>());
        CHECK(uarg{PRV_PTR()}.can_cast_to<int* const>());
        CHECK(uarg{PRV_PTR()}.can_cast_to<const int*>());
        CHECK(uarg{PRV_PTR()}.can_cast_to<const int* const>());

        CHECK(*uarg{PRV_PTR()}.cast<int*>() == 42);
        CHECK(*uarg{PRV_PTR()}.cast<int* const>() == 42);
        CHECK(*uarg{PRV_PTR()}.cast<const int*>() == 42);
        CHECK(*uarg{PRV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("LV_CPTR") {
        CHECK_FALSE(uarg{LV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(uarg{LV_CPTR()}.can_cast_to<int* const>());
        CHECK(uarg{LV_CPTR()}.can_cast_to<const int*>());
        CHECK(uarg{LV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = uarg{LV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = uarg{LV_CPTR()}.cast<int* const>());
        CHECK(*uarg{LV_CPTR()}.cast<const int*>() == 42);
        CHECK(*uarg{LV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CLV_CPTR") {
        CHECK_FALSE(uarg{CLV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(uarg{CLV_CPTR()}.can_cast_to<int* const>());
        CHECK(uarg{CLV_CPTR()}.can_cast_to<const int*>());
        CHECK(uarg{CLV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = uarg{CLV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = uarg{CLV_CPTR()}.cast<int* const>());
        CHECK(*uarg{CLV_CPTR()}.cast<const int*>() == 42);
        CHECK(*uarg{CLV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("XV_CPTR") {
        CHECK_FALSE(uarg{XV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(uarg{XV_CPTR()}.can_cast_to<int* const>());
        CHECK(uarg{XV_CPTR()}.can_cast_to<const int*>());
        CHECK(uarg{XV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = uarg{XV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = uarg{XV_CPTR()}.cast<int* const>());
        CHECK(*uarg{XV_CPTR()}.cast<const int*>() == 42);
        CHECK(*uarg{XV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CXV_CPTR") {
        CHECK_FALSE(uarg{CXV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(uarg{CXV_CPTR()}.can_cast_to<int* const>());
        CHECK(uarg{CXV_CPTR()}.can_cast_to<const int*>());
        CHECK(uarg{CXV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = uarg{CXV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = uarg{CXV_CPTR()}.cast<int* const>());
        CHECK(*uarg{CXV_CPTR()}.cast<const int*>() == 42);
        CHECK(*uarg{CXV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("PRV_CPTR") {
        CHECK_FALSE(uarg{PRV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(uarg{PRV_CPTR()}.can_cast_to<int* const>());
        CHECK(uarg{PRV_CPTR()}.can_cast_to<const int*>());
        CHECK(uarg{PRV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = uarg{PRV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = uarg{PRV_CPTR()}.cast<int* const>());
        CHECK(*uarg{PRV_CPTR()}.cast<const int*>() == 42);
        CHECK(*uarg{PRV_CPTR()}.cast<const int* const>() == 42);
    }
}
