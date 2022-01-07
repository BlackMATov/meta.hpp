/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

TEST_CASE("features/meta_utilities/arg4/cast") {
    namespace meta = meta_hpp;
    using meta::detail::arg;

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
        CHECK(arg{LV_PTR()}.can_cast_to<int*>());
        CHECK(arg{LV_PTR()}.can_cast_to<int* const>());
        CHECK(arg{LV_PTR()}.can_cast_to<const int*>());
        CHECK(arg{LV_PTR()}.can_cast_to<const int* const>());

        CHECK(*arg{LV_PTR()}.cast<int*>() == 42);
        CHECK(*arg{LV_PTR()}.cast<int* const>() == 42);
        CHECK(*arg{LV_PTR()}.cast<const int*>() == 42);
        CHECK(*arg{LV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CLV_PTR") {
        CHECK(arg{CLV_PTR()}.can_cast_to<int*>());
        CHECK(arg{CLV_PTR()}.can_cast_to<int* const>());
        CHECK(arg{CLV_PTR()}.can_cast_to<const int*>());
        CHECK(arg{CLV_PTR()}.can_cast_to<const int* const>());

        CHECK(*arg{CLV_PTR()}.cast<int*>() == 42);
        CHECK(*arg{CLV_PTR()}.cast<int* const>() == 42);
        CHECK(*arg{CLV_PTR()}.cast<const int*>() == 42);
        CHECK(*arg{CLV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("XV_PTR") {
        CHECK(arg{XV_PTR()}.can_cast_to<int*>());
        CHECK(arg{XV_PTR()}.can_cast_to<int* const>());
        CHECK(arg{XV_PTR()}.can_cast_to<const int*>());
        CHECK(arg{XV_PTR()}.can_cast_to<const int* const>());

        CHECK(*arg{XV_PTR()}.cast<int*>() == 42);
        CHECK(*arg{XV_PTR()}.cast<int* const>() == 42);
        CHECK(*arg{XV_PTR()}.cast<const int*>() == 42);
        CHECK(*arg{XV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CXV_PTR") {
        CHECK(arg{CXV_PTR()}.can_cast_to<int*>());
        CHECK(arg{CXV_PTR()}.can_cast_to<int* const>());
        CHECK(arg{CXV_PTR()}.can_cast_to<const int*>());
        CHECK(arg{CXV_PTR()}.can_cast_to<const int* const>());

        CHECK(*arg{CXV_PTR()}.cast<int*>() == 42);
        CHECK(*arg{CXV_PTR()}.cast<int* const>() == 42);
        CHECK(*arg{CXV_PTR()}.cast<const int*>() == 42);
        CHECK(*arg{CXV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("PRV_PTR") {
        CHECK(arg{PRV_PTR()}.can_cast_to<int*>());
        CHECK(arg{PRV_PTR()}.can_cast_to<int* const>());
        CHECK(arg{PRV_PTR()}.can_cast_to<const int*>());
        CHECK(arg{PRV_PTR()}.can_cast_to<const int* const>());

        CHECK(*arg{PRV_PTR()}.cast<int*>() == 42);
        CHECK(*arg{PRV_PTR()}.cast<int* const>() == 42);
        CHECK(*arg{PRV_PTR()}.cast<const int*>() == 42);
        CHECK(*arg{PRV_PTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("LV_CPTR") {
        CHECK_FALSE(arg{LV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(arg{LV_CPTR()}.can_cast_to<int* const>());
        CHECK(arg{LV_CPTR()}.can_cast_to<const int*>());
        CHECK(arg{LV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = arg{LV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = arg{LV_CPTR()}.cast<int* const>());
        CHECK(*arg{LV_CPTR()}.cast<const int*>() == 42);
        CHECK(*arg{LV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CLV_CPTR") {
        CHECK_FALSE(arg{CLV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(arg{CLV_CPTR()}.can_cast_to<int* const>());
        CHECK(arg{CLV_CPTR()}.can_cast_to<const int*>());
        CHECK(arg{CLV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = arg{CLV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = arg{CLV_CPTR()}.cast<int* const>());
        CHECK(*arg{CLV_CPTR()}.cast<const int*>() == 42);
        CHECK(*arg{CLV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("XV_CPTR") {
        CHECK_FALSE(arg{XV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(arg{XV_CPTR()}.can_cast_to<int* const>());
        CHECK(arg{XV_CPTR()}.can_cast_to<const int*>());
        CHECK(arg{XV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = arg{XV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = arg{XV_CPTR()}.cast<int* const>());
        CHECK(*arg{XV_CPTR()}.cast<const int*>() == 42);
        CHECK(*arg{XV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("CXV_CPTR") {
        CHECK_FALSE(arg{CXV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(arg{CXV_CPTR()}.can_cast_to<int* const>());
        CHECK(arg{CXV_CPTR()}.can_cast_to<const int*>());
        CHECK(arg{CXV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = arg{CXV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = arg{CXV_CPTR()}.cast<int* const>());
        CHECK(*arg{CXV_CPTR()}.cast<const int*>() == 42);
        CHECK(*arg{CXV_CPTR()}.cast<const int* const>() == 42);
    }

    SUBCASE("PRV_CPTR") {
        CHECK_FALSE(arg{PRV_CPTR()}.can_cast_to<int*>());
        CHECK_FALSE(arg{PRV_CPTR()}.can_cast_to<int* const>());
        CHECK(arg{PRV_CPTR()}.can_cast_to<const int*>());
        CHECK(arg{PRV_CPTR()}.can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = arg{PRV_CPTR()}.cast<int*>());
        CHECK_THROWS(std::ignore = arg{PRV_CPTR()}.cast<int* const>());
        CHECK(*arg{PRV_CPTR()}.cast<const int*>() == 42);
        CHECK(*arg{PRV_CPTR()}.cast<const int* const>() == 42);
    }
}
