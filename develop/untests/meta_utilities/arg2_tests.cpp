/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A {
        A() = default;
        virtual ~A() = default;

        A(A&&) noexcept { ++move_ctors_; }
        A(const A&) { ++copy_ctors_; }

        A& operator=(A&&) = delete;
        A& operator=(const A&) = delete;

        int i = 1;
        [[nodiscard]] int f() const { return i; }

        static int copy_ctors_;
        static int move_ctors_;
    };

    int A::copy_ctors_{};
    int A::move_ctors_{};

    struct B : virtual A {
        int bi = 2;
        [[nodiscard]] int f() const { return bi; }
        META_HPP_ENABLE_BASE_INFO(A)
    };

    struct C : virtual A {
        int ci = 3;
        [[nodiscard]] int f() const { return ci; }
        META_HPP_ENABLE_BASE_INFO(A)
    };

    struct D : B, C {
        int di = 4;
        [[nodiscard]] int f() const { return di; }
        META_HPP_ENABLE_BASE_INFO(B, C)
    };
}

TEST_CASE("meta/meta_utilities/arg2") {
    namespace meta = meta_hpp;

    //  * <- B <- *
    // A           D
    //  * <- C <- *
}

TEST_CASE("meta/meta_utilities/arg2/cast") {
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

    auto LV_PTR = []() -> D*& { static D v; static D* p{&v}; return p; };
    auto LV_CPTR = []() -> const D*& { static D v; static const D* p{&v}; return p; };
    auto CLV_PTR = []() -> D* const& { static D v; static D* p{&v}; return p; };
    auto CLV_CPTR = []() -> const D* const& { static D v; static const D* p{&v}; return p; };
    auto XV_PTR = []() -> D*&& { static D v; static D* p{&v}; return std::move(p); };
    auto XV_CPTR = []() -> const D*&& { static D v; static const D* p{&v}; return std::move(p); };
    auto CXV_PTR = []() -> D* const&& { static D v; static D* p{&v}; return std::move(p); };
    auto CXV_CPTR = []() -> const D* const&& { static D v; static const D* p{&v}; return std::move(p); };
    auto PRV_PTR = []() -> D* { static D v; static D* p{&v}; return p; };
    auto PRV_CPTR = []() -> const D* { static D v; static const D* p{&v}; return p; };

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

    A::copy_ctors_ = 0;
    A::move_ctors_ = 0;

    SUBCASE("LV") {
        CHECK(uarg{r, LV()}.can_cast_to<A>(r));
        CHECK(uarg{r, LV()}.can_cast_to<const A>(r));
        CHECK(uarg{r, LV()}.can_cast_to<A&>(r));
        CHECK(uarg{r, LV()}.can_cast_to<const A&>(r));
        CHECK_FALSE(uarg{r, LV()}.can_cast_to<A&&>(r));
        CHECK_FALSE(uarg{r, LV()}.can_cast_to<const A&&>(r));

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{r, LV()}.cast<A>(r).f() == 1);
        CHECK(uarg{r, LV()}.cast<const A>(r).f() == 1);
        CHECK(uarg{r, LV()}.cast<A&>(r).f() == 1);
        CHECK(uarg{r, LV()}.cast<const A&>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, LV()}.cast<A&&>(r));
        // CHECK_THROWS(std::ignore = uarg{r, LV()}.cast<const A&&>(r));

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("CLV") {
        CHECK(uarg{r, CLV()}.can_cast_to<A>(r));
        CHECK(uarg{r, CLV()}.can_cast_to<const A>(r));
        CHECK_FALSE(uarg{r, CLV()}.can_cast_to<A&>(r));
        CHECK(uarg{r, CLV()}.can_cast_to<const A&>(r));
        CHECK_FALSE(uarg{r, CLV()}.can_cast_to<A&&>(r));
        CHECK_FALSE(uarg{r, CLV()}.can_cast_to<const A&&>(r));

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{r, CLV()}.cast<A>(r).f() == 1);
        CHECK(uarg{r, CLV()}.cast<const A>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, CLV()}.cast<A&>(r));
        CHECK(uarg{r, CLV()}.cast<const A&>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, CLV()}.cast<A&&>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CLV()}.cast<const A&&>(r));

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("XV") {
        CHECK(uarg{r, XV()}.can_cast_to<A>(r));
        CHECK(uarg{r, XV()}.can_cast_to<const A>(r));
        CHECK_FALSE(uarg{r, XV()}.can_cast_to<A&>(r));
        CHECK(uarg{r, XV()}.can_cast_to<const A&>(r));
        CHECK(uarg{r, XV()}.can_cast_to<A&&>(r));
        CHECK(uarg{r, XV()}.can_cast_to<const A&&>(r));

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{r, XV()}.cast<A>(r).f() == 1);
        CHECK(uarg{r, XV()}.cast<const A>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, XV()}.cast<A&>(r));
        CHECK(uarg{r, XV()}.cast<const A&>(r).f() == 1);
        CHECK(uarg{r, XV()}.cast<A&&>(r).f() == 1);
        CHECK(uarg{r, XV()}.cast<const A&&>(r).f() == 1);

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 2);
    }

    SUBCASE("CXV") {
        CHECK(uarg{r, CXV()}.can_cast_to<A>(r));
        CHECK(uarg{r, CXV()}.can_cast_to<const A>(r));
        CHECK_FALSE(uarg{r, CXV()}.can_cast_to<A&>(r));
        CHECK(uarg{r, CXV()}.can_cast_to<const A&>(r));
        CHECK_FALSE(uarg{r, CXV()}.can_cast_to<A&&>(r));
        CHECK(uarg{r, CXV()}.can_cast_to<const A&&>(r));

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{r, CXV()}.cast<A>(r).f() == 1);
        CHECK(uarg{r, CXV()}.cast<const A>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, CXV()}.cast<A&>(r));
        CHECK(uarg{r, CXV()}.cast<const A&>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, CXV()}.cast<A&&>(r));
        CHECK(uarg{r, CXV()}.cast<const A&&>(r).f() == 1);

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("PRV") {
        CHECK(uarg{r, PRV()}.can_cast_to<A>(r));
        CHECK(uarg{r, PRV()}.can_cast_to<const A>(r));
        CHECK_FALSE(uarg{r, PRV()}.can_cast_to<A&>(r));
        CHECK(uarg{r, PRV()}.can_cast_to<const A&>(r));
        CHECK(uarg{r, PRV()}.can_cast_to<A&&>(r));
        CHECK(uarg{r, PRV()}.can_cast_to<const A&&>(r));

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{r, PRV()}.cast<A>(r).f() == 1);
        CHECK(uarg{r, PRV()}.cast<const A>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, PRV()}.cast<A&>(r));
        CHECK(uarg{r, PRV()}.cast<const A&>(r).f() == 1);
        CHECK(uarg{r, PRV()}.cast<A&&>(r).f() == 1);
        CHECK(uarg{r, PRV()}.cast<const A&&>(r).f() == 1);

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 2);
    }

    SUBCASE("CPRV") {
        CHECK(uarg{r, CPRV()}.can_cast_to<A>(r));
        CHECK(uarg{r, CPRV()}.can_cast_to<const A>(r));
        CHECK_FALSE(uarg{r, CPRV()}.can_cast_to<A&>(r));
        CHECK(uarg{r, CPRV()}.can_cast_to<const A&>(r));
        CHECK_FALSE(uarg{r, CPRV()}.can_cast_to<A&&>(r));
        CHECK(uarg{r, CPRV()}.can_cast_to<const A&&>(r));

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{r, CPRV()}.cast<A>(r).f() == 1);
        CHECK(uarg{r, CPRV()}.cast<const A>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, CPRV()}.cast<A&>(r));
        CHECK(uarg{r, CPRV()}.cast<const A&>(r).f() == 1);
        // CHECK_THROWS(std::ignore = uarg{r, CPRV()}.cast<A&&>(r));
        CHECK(uarg{r, CPRV()}.cast<const A&&>(r).f() == 1);

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("LV_PTR") {
        CHECK(uarg{r, LV_PTR()}.can_cast_to<A*>(r));
        CHECK(uarg{r, LV_PTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, LV_PTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, LV_PTR()}.can_cast_to<const A* const>(r));

        CHECK(uarg{r, LV_PTR()}.cast<A*>(r)->f() == 1);
        CHECK(uarg{r, LV_PTR()}.cast<A* const>(r)->f() == 1);
        CHECK(uarg{r, LV_PTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, LV_PTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("LV_CPTR") {
        CHECK_FALSE(uarg{r, LV_CPTR()}.can_cast_to<A*>(r));
        CHECK_FALSE(uarg{r, LV_CPTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, LV_CPTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, LV_CPTR()}.can_cast_to<const A* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, LV_CPTR()}.cast<A*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, LV_CPTR()}.cast<A* const>(r));
        CHECK(uarg{r, LV_CPTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, LV_CPTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("CLV_PTR") {
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<A*>(r));
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, CLV_PTR()}.can_cast_to<const A* const>(r));

        CHECK(uarg{r, CLV_PTR()}.cast<A*>(r)->f() == 1);
        CHECK(uarg{r, CLV_PTR()}.cast<A* const>(r)->f() == 1);
        CHECK(uarg{r, CLV_PTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, CLV_PTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("CLV_CPTR") {
        CHECK_FALSE(uarg{r, CLV_CPTR()}.can_cast_to<A*>(r));
        CHECK_FALSE(uarg{r, CLV_CPTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, CLV_CPTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, CLV_CPTR()}.can_cast_to<const A* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, CLV_CPTR()}.cast<A*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CLV_CPTR()}.cast<A* const>(r));
        CHECK(uarg{r, CLV_CPTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, CLV_CPTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("XV_PTR") {
        CHECK(uarg{r, XV_PTR()}.can_cast_to<A*>(r));
        CHECK(uarg{r, XV_PTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, XV_PTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, XV_PTR()}.can_cast_to<const A* const>(r));

        CHECK(uarg{r, XV_PTR()}.cast<A*>(r)->f() == 1);
        CHECK(uarg{r, XV_PTR()}.cast<A* const>(r)->f() == 1);
        CHECK(uarg{r, XV_PTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, XV_PTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("XV_CPTR") {
        CHECK_FALSE(uarg{r, XV_CPTR()}.can_cast_to<A*>(r));
        CHECK_FALSE(uarg{r, XV_CPTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, XV_CPTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, XV_CPTR()}.can_cast_to<const A* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, XV_CPTR()}.cast<A*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, XV_CPTR()}.cast<A* const>(r));
        CHECK(uarg{r, XV_CPTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, XV_CPTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("CXV_PTR") {
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<A*>(r));
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, CXV_PTR()}.can_cast_to<const A* const>(r));

        CHECK(uarg{r, CXV_PTR()}.cast<A*>(r)->f() == 1);
        CHECK(uarg{r, CXV_PTR()}.cast<A* const>(r)->f() == 1);
        CHECK(uarg{r, CXV_PTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, CXV_PTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("CXV_CPTR") {
        CHECK_FALSE(uarg{r, CXV_CPTR()}.can_cast_to<A*>(r));
        CHECK_FALSE(uarg{r, CXV_CPTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, CXV_CPTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, CXV_CPTR()}.can_cast_to<const A* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, CXV_CPTR()}.cast<A*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, CXV_CPTR()}.cast<A* const>(r));
        CHECK(uarg{r, CXV_CPTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, CXV_CPTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("PRV_PTR") {
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<A*>(r));
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, PRV_PTR()}.can_cast_to<const A* const>(r));

        CHECK(uarg{r, PRV_PTR()}.cast<A*>(r)->f() == 1);
        CHECK(uarg{r, PRV_PTR()}.cast<A* const>(r)->f() == 1);
        CHECK(uarg{r, PRV_PTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, PRV_PTR()}.cast<const A* const>(r)->f() == 1);
    }

    SUBCASE("PRV_CPTR") {
        CHECK_FALSE(uarg{r, PRV_CPTR()}.can_cast_to<A*>(r));
        CHECK_FALSE(uarg{r, PRV_CPTR()}.can_cast_to<A* const>(r));
        CHECK(uarg{r, PRV_CPTR()}.can_cast_to<const A*>(r));
        CHECK(uarg{r, PRV_CPTR()}.can_cast_to<const A* const>(r));

        // CHECK_THROWS(std::ignore = uarg{r, PRV_CPTR()}.cast<A*>(r));
        // CHECK_THROWS(std::ignore = uarg{r, PRV_CPTR()}.cast<A* const>(r));
        CHECK(uarg{r, PRV_CPTR()}.cast<const A*>(r)->f() == 1);
        CHECK(uarg{r, PRV_CPTR()}.cast<const A* const>(r)->f() == 1);
    }
}
