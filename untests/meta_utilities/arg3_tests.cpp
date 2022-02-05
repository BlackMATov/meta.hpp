/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

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
        [[maybe_unused, nodiscard]] int f() const { return i; }
    };

    struct B : virtual A {
        int bi = 2;
        [[maybe_unused, nodiscard]] int f() const { return bi; }
    };

    struct C : virtual A {
        int ci = 3;
        [[maybe_unused, nodiscard]] int f() const { return ci; }
    };

    struct D : B, C {
        int di = 4;
        [[maybe_unused, nodiscard]] int f() const { return di; }
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
    using meta::detail::arg;

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
        CHECK_FALSE(arg{LV()}.can_cast_to<A>());
        CHECK_FALSE(arg{LV()}.can_cast_to<const A>());

        CHECK_THROWS(std::ignore = arg{LV()}.cast<A>());
        CHECK_THROWS(std::ignore = arg{LV()}.cast<const A>());
    }

    SUBCASE("CLV") {
        CHECK_FALSE(arg{CLV()}.can_cast_to<A>());
        CHECK_FALSE(arg{CLV()}.can_cast_to<const A>());

        CHECK_THROWS(std::ignore = arg{CLV()}.cast<A>());
        CHECK_THROWS(std::ignore = arg{CLV()}.cast<const A>());
    }

    SUBCASE("XV") {
        CHECK_FALSE(arg{XV()}.can_cast_to<A>());
        CHECK_FALSE(arg{XV()}.can_cast_to<const A>());

        CHECK_THROWS(std::ignore = arg{XV()}.cast<A>());
        CHECK_THROWS(std::ignore = arg{XV()}.cast<const A>());
    }

    SUBCASE("CXV") {
        CHECK_FALSE(arg{CXV()}.can_cast_to<A>());
        CHECK_FALSE(arg{CXV()}.can_cast_to<const A>());

        CHECK_THROWS(std::ignore = arg{CXV()}.cast<A>());
        CHECK_THROWS(std::ignore = arg{CXV()}.cast<const A>());
    }

    SUBCASE("PRV") {
        CHECK_FALSE(arg{PRV()}.can_cast_to<A>());
        CHECK_FALSE(arg{PRV()}.can_cast_to<const A>());

        CHECK_THROWS(std::ignore = arg{PRV()}.cast<A>());
        CHECK_THROWS(std::ignore = arg{PRV()}.cast<const A>());
    }

    SUBCASE("CPRV") {
        CHECK_FALSE(arg{CPRV()}.can_cast_to<A>());
        CHECK_FALSE(arg{CPRV()}.can_cast_to<const A>());

        CHECK_THROWS(std::ignore = arg{CPRV()}.cast<A>());
        CHECK_THROWS(std::ignore = arg{CPRV()}.cast<const A>());
    }
}
