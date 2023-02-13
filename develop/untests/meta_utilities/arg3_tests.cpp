/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A {
        A() = default;
        virtual ~A() = default;

        A(A&&) = delete;
        A(const A&) = delete;
        A& operator=(A&&) = delete;
        A& operator=(const A&) = delete;

        int i = 1;
        [[nodiscard]] int f() const { return i; }
    };

    struct B : virtual A {
        int bi = 2;
        [[nodiscard]] int f() const { return bi; }
    };

    struct C : virtual A {
        int ci = 3;
        [[nodiscard]] int f() const { return ci; }
    };

    struct D : B, C {
        int di = 4;
        [[nodiscard]] int f() const { return di; }
    };
}

TEST_CASE("meta/meta_utilities/arg3") {
    namespace meta = meta_hpp;

    //  * <- B <- *
    // A           D
    //  * <- C <- *

    meta::class_<A>();
    meta::class_<B>().base_<A>();
    meta::class_<C>().base_<A>();
    meta::class_<D>().base_<B>().base_<C>();
}

TEST_CASE("meta/meta_utilities/arg3/cast") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    auto LV = []() -> D& { static D v; return v; };
    auto CLV = []() -> const D& { static D v; return v; };
    auto XV = []() -> D&& { static D v; return std::move(v); };
    auto CXV = []() -> const D&& { static D v; return std::move(v); };
    auto PRV = []() -> D { return D{}; };
    auto CPRV = []() -> const D { return D{}; };

    // *------------------------------------------------------------*
    // | ======> | T, const T |  T&  | const T& | T&&  | const T&&  |
    // |---------*------------*------*----------*------*------------|
    // | LV      |     Cc     |  ++  |    ++    |      |            |
    // |---------*------------*------*----------*------*------------|
    // | CLV     |     Cc     |      |    ++    |      |            |
    // |---------*------------*------*----------*------*------------|
    // | XV      |     Mc     |      |    ++    |  ++  |     ++     |
    // |---------*------------*------*----------*------*------------|
    // | CXV     |     Cc     |      |    ++    |      |     ++     |
    // |---------*------------*------*----------*------*------------|
    // | PRV     |     ++     |      |    ++    |  ++  |     ++     |
    // |---------*------------*------*----------*------*------------|
    // | CPRV    |     ++     |      |    ++    |      |     ++     |
    // *------------------------------------------------------------*

    SUBCASE("LV") {
        CHECK_FALSE(uarg{r, LV()}.can_cast_to<A>(r));
        CHECK_FALSE(uarg{r, LV()}.can_cast_to<const A>(r));

        // CHECK_THROWS(std::ignore = uarg{r, LV()}.cast<A>(r));
        // CHECK_THROWS(std::ignore = uarg{r, LV()}.cast<const A>(r));
    }

    SUBCASE("CLV") {
        CHECK_FALSE(uarg{r, CLV()}.can_cast_to<A>(r));
        CHECK_FALSE(uarg{r, CLV()}.can_cast_to<const A>(r));

        // CHECK_THROWS(std::ignore = uarg{r, CLV()}.cast<A>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CLV()}.cast<const A>(r));
    }

    SUBCASE("XV") {
        CHECK_FALSE(uarg{r, XV()}.can_cast_to<A>(r));
        CHECK_FALSE(uarg{r, XV()}.can_cast_to<const A>(r));

        // CHECK_THROWS(std::ignore = uarg{r, XV()}.cast<A>(r));
        // CHECK_THROWS(std::ignore = uarg{r, XV()}.cast<const A>(r));
    }

    SUBCASE("CXV") {
        CHECK_FALSE(uarg{r, CXV()}.can_cast_to<A>(r));
        CHECK_FALSE(uarg{r, CXV()}.can_cast_to<const A>(r));

        // CHECK_THROWS(std::ignore = uarg{r, CXV()}.cast<A>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CXV()}.cast<const A>(r));
    }

    SUBCASE("PRV") {
        CHECK_FALSE(uarg{r, PRV()}.can_cast_to<A>(r));
        CHECK_FALSE(uarg{r, PRV()}.can_cast_to<const A>(r));

        // CHECK_THROWS(std::ignore = uarg{r, PRV()}.cast<A>(r));
        // CHECK_THROWS(std::ignore = uarg{r, PRV()}.cast<const A>(r));
    }

    SUBCASE("CPRV") {
        CHECK_FALSE(uarg{r, CPRV()}.can_cast_to<A>(r));
        CHECK_FALSE(uarg{r, CPRV()}.can_cast_to<const A>(r));

        // CHECK_THROWS(std::ignore = uarg{r, CPRV()}.cast<A>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CPRV()}.cast<const A>(r));
    }
}
