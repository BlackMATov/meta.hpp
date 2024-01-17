/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_utilities/arg4/cast") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

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
        CHECK(uarg{r, LV_PTR()}.can_cast_to<int*>(r));
        CHECK(uarg{r, LV_PTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, LV_PTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, LV_PTR()}.can_cast_to<const int* const>(r));

        CHECK(*uarg{r, LV_PTR()}.cast<int*>(r) == 42);
        CHECK(*uarg{r, LV_PTR()}.cast<int* const>(r) == 42);
        CHECK(*uarg{r, LV_PTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, LV_PTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("CLV_PTR") {
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<int*>(r));
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<const int* const>(r));

        CHECK(*uarg{r, CLV_PTR()}.cast<int*>(r) == 42);
        CHECK(*uarg{r, CLV_PTR()}.cast<int* const>(r) == 42);
        CHECK(*uarg{r, CLV_PTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, CLV_PTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("XV_PTR") {
        CHECK(uarg{r, XV_PTR()}.can_cast_to<int*>(r));
        CHECK(uarg{r, XV_PTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, XV_PTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, XV_PTR()}.can_cast_to<const int* const>(r));

        CHECK(*uarg{r, XV_PTR()}.cast<int*>(r) == 42);
        CHECK(*uarg{r, XV_PTR()}.cast<int* const>(r) == 42);
        CHECK(*uarg{r, XV_PTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, XV_PTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("CXV_PTR") {
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<int*>(r));
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<const int* const>(r));

        CHECK(*uarg{r, CXV_PTR()}.cast<int*>(r) == 42);
        CHECK(*uarg{r, CXV_PTR()}.cast<int* const>(r) == 42);
        CHECK(*uarg{r, CXV_PTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, CXV_PTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("PRV_PTR") {
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<int*>(r));
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<const int* const>(r));

        CHECK(*uarg{r, PRV_PTR()}.cast<int*>(r) == 42);
        CHECK(*uarg{r, PRV_PTR()}.cast<int* const>(r) == 42);
        CHECK(*uarg{r, PRV_PTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, PRV_PTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("LV_CPTR") {
        CHECK_FALSE(uarg{r, LV_CPTR()}.can_cast_to<int*>(r));
        CHECK_FALSE(uarg{r, LV_CPTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, LV_CPTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, LV_CPTR()}.can_cast_to<const int* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, LV_CPTR()}.cast<int*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, LV_CPTR()}.cast<int* const>(r));
        CHECK(*uarg{r, LV_CPTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, LV_CPTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("CLV_CPTR") {
        CHECK_FALSE(uarg{r, CLV_CPTR()}.can_cast_to<int*>(r));
        CHECK_FALSE(uarg{r, CLV_CPTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, CLV_CPTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, CLV_CPTR()}.can_cast_to<const int* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, CLV_CPTR()}.cast<int*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CLV_CPTR()}.cast<int* const>(r));
        CHECK(*uarg{r, CLV_CPTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, CLV_CPTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("XV_CPTR") {
        CHECK_FALSE(uarg{r, XV_CPTR()}.can_cast_to<int*>(r));
        CHECK_FALSE(uarg{r, XV_CPTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, XV_CPTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, XV_CPTR()}.can_cast_to<const int* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, XV_CPTR()}.cast<int*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, XV_CPTR()}.cast<int* const>(r));
        CHECK(*uarg{r, XV_CPTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, XV_CPTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("CXV_CPTR") {
        CHECK_FALSE(uarg{r, CXV_CPTR()}.can_cast_to<int*>(r));
        CHECK_FALSE(uarg{r, CXV_CPTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, CXV_CPTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, CXV_CPTR()}.can_cast_to<const int* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, CXV_CPTR()}.cast<int*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CXV_CPTR()}.cast<int* const>(r));
        CHECK(*uarg{r, CXV_CPTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, CXV_CPTR()}.cast<const int* const>(r) == 42);
    }

    SUBCASE("PRV_CPTR") {
        CHECK_FALSE(uarg{r, PRV_CPTR()}.can_cast_to<int*>(r));
        CHECK_FALSE(uarg{r, PRV_CPTR()}.can_cast_to<int* const>(r));
        CHECK(uarg{r, PRV_CPTR()}.can_cast_to<const int*>(r));
        CHECK(uarg{r, PRV_CPTR()}.can_cast_to<const int* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, PRV_CPTR()}.cast<int*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, PRV_CPTR()}.cast<int* const>(r));
        CHECK(*uarg{r, PRV_CPTR()}.cast<const int*>(r) == 42);
        CHECK(*uarg{r, PRV_CPTR()}.cast<const int* const>(r) == 42);
    }
}
