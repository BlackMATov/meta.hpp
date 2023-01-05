/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct A {
        A() = default;
        virtual ~A() = default;

        [[maybe_unused]] A(A&&) noexcept { ++move_ctors_; }
        [[maybe_unused]] A(const A&) { ++copy_ctors_; }

        A& operator=(A&&) = delete;
        A& operator=(const A&) = delete;

        int i = 1;
        [[maybe_unused, nodiscard]] int f() const { return i; }

        static int copy_ctors_;
        static int move_ctors_;
    };

    int A::copy_ctors_{};
    int A::move_ctors_{};

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

TEST_CASE("meta/meta_utilities/arg2") {
    namespace meta = meta_hpp;

    //  * <- B <- *
    // A           D
    //  * <- C <- *

    meta::class_<A>();
    meta::class_<B>().base_<A>();
    meta::class_<C>().base_<A>();
    meta::class_<D>().base_<B>().base_<C>();
}

TEST_CASE("meta/meta_utilities/arg2/cast") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;

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
        CHECK(uarg{LV()}.can_cast_to<A>());
        CHECK(uarg{LV()}.can_cast_to<const A>());
        CHECK(uarg{LV()}.can_cast_to<A&>());
        CHECK(uarg{LV()}.can_cast_to<const A&>());
        CHECK_FALSE(uarg{LV()}.can_cast_to<A&&>());
        CHECK_FALSE(uarg{LV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{LV()}.cast<A>().f() == 1);
        CHECK(uarg{LV()}.cast<const A>().f() == 1);
        CHECK(uarg{LV()}.cast<A&>().f() == 1);
        CHECK(uarg{LV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{LV()}.cast<A&&>());
        CHECK_THROWS(std::ignore = uarg{LV()}.cast<const A&&>());

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("CLV") {
        CHECK(uarg{CLV()}.can_cast_to<A>());
        CHECK(uarg{CLV()}.can_cast_to<const A>());
        CHECK_FALSE(uarg{CLV()}.can_cast_to<A&>());
        CHECK(uarg{CLV()}.can_cast_to<const A&>());
        CHECK_FALSE(uarg{CLV()}.can_cast_to<A&&>());
        CHECK_FALSE(uarg{CLV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{CLV()}.cast<A>().f() == 1);
        CHECK(uarg{CLV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{CLV()}.cast<A&>());
        CHECK(uarg{CLV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{CLV()}.cast<A&&>());
        CHECK_THROWS(std::ignore = uarg{CLV()}.cast<const A&&>());

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("XV") {
        CHECK(uarg{XV()}.can_cast_to<A>());
        CHECK(uarg{XV()}.can_cast_to<const A>());
        CHECK_FALSE(uarg{XV()}.can_cast_to<A&>());
        CHECK(uarg{XV()}.can_cast_to<const A&>());
        CHECK(uarg{XV()}.can_cast_to<A&&>());
        CHECK(uarg{XV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{XV()}.cast<A>().f() == 1);
        CHECK(uarg{XV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{XV()}.cast<A&>());
        CHECK(uarg{XV()}.cast<const A&>().f() == 1);
        CHECK(uarg{XV()}.cast<A&&>().f() == 1);
        CHECK(uarg{XV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 2);
    }

    SUBCASE("CXV") {
        CHECK(uarg{CXV()}.can_cast_to<A>());
        CHECK(uarg{CXV()}.can_cast_to<const A>());
        CHECK_FALSE(uarg{CXV()}.can_cast_to<A&>());
        CHECK(uarg{CXV()}.can_cast_to<const A&>());
        CHECK_FALSE(uarg{CXV()}.can_cast_to<A&&>());
        CHECK(uarg{CXV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{CXV()}.cast<A>().f() == 1);
        CHECK(uarg{CXV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{CXV()}.cast<A&>());
        CHECK(uarg{CXV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{CXV()}.cast<A&&>());
        CHECK(uarg{CXV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("PRV") {
        CHECK(uarg{PRV()}.can_cast_to<A>());
        CHECK(uarg{PRV()}.can_cast_to<const A>());
        CHECK_FALSE(uarg{PRV()}.can_cast_to<A&>());
        CHECK(uarg{PRV()}.can_cast_to<const A&>());
        CHECK(uarg{PRV()}.can_cast_to<A&&>());
        CHECK(uarg{PRV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{PRV()}.cast<A>().f() == 1);
        CHECK(uarg{PRV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{PRV()}.cast<A&>());
        CHECK(uarg{PRV()}.cast<const A&>().f() == 1);
        CHECK(uarg{PRV()}.cast<A&&>().f() == 1);
        CHECK(uarg{PRV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 2);
    }

    SUBCASE("CPRV") {
        CHECK(uarg{CPRV()}.can_cast_to<A>());
        CHECK(uarg{CPRV()}.can_cast_to<const A>());
        CHECK_FALSE(uarg{CPRV()}.can_cast_to<A&>());
        CHECK(uarg{CPRV()}.can_cast_to<const A&>());
        CHECK_FALSE(uarg{CPRV()}.can_cast_to<A&&>());
        CHECK(uarg{CPRV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(uarg{CPRV()}.cast<A>().f() == 1);
        CHECK(uarg{CPRV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{CPRV()}.cast<A&>());
        CHECK(uarg{CPRV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = uarg{CPRV()}.cast<A&&>());
        CHECK(uarg{CPRV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("LV_PTR") {
        CHECK(uarg{LV_PTR()}.can_cast_to<A*>());
        CHECK(uarg{LV_PTR()}.can_cast_to<A* const>());
        CHECK(uarg{LV_PTR()}.can_cast_to<const A*>());
        CHECK(uarg{LV_PTR()}.can_cast_to<const A* const>());

        CHECK(uarg{LV_PTR()}.cast<A*>()->f() == 1);
        CHECK(uarg{LV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(uarg{LV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{LV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("LV_CPTR") {
        CHECK_FALSE(uarg{LV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(uarg{LV_CPTR()}.can_cast_to<A* const>());
        CHECK(uarg{LV_CPTR()}.can_cast_to<const A*>());
        CHECK(uarg{LV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = uarg{LV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = uarg{LV_CPTR()}.cast<A* const>());
        CHECK(uarg{LV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{LV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CLV_PTR") {
        CHECK(uarg{CLV_PTR()}.can_cast_to<A*>());
        CHECK(uarg{CLV_PTR()}.can_cast_to<A* const>());
        CHECK(uarg{CLV_PTR()}.can_cast_to<const A*>());
        CHECK(uarg{CLV_PTR()}.can_cast_to<const A* const>());

        CHECK(uarg{CLV_PTR()}.cast<A*>()->f() == 1);
        CHECK(uarg{CLV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(uarg{CLV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{CLV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CLV_CPTR") {
        CHECK_FALSE(uarg{CLV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(uarg{CLV_CPTR()}.can_cast_to<A* const>());
        CHECK(uarg{CLV_CPTR()}.can_cast_to<const A*>());
        CHECK(uarg{CLV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = uarg{CLV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = uarg{CLV_CPTR()}.cast<A* const>());
        CHECK(uarg{CLV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{CLV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("XV_PTR") {
        CHECK(uarg{XV_PTR()}.can_cast_to<A*>());
        CHECK(uarg{XV_PTR()}.can_cast_to<A* const>());
        CHECK(uarg{XV_PTR()}.can_cast_to<const A*>());
        CHECK(uarg{XV_PTR()}.can_cast_to<const A* const>());

        CHECK(uarg{XV_PTR()}.cast<A*>()->f() == 1);
        CHECK(uarg{XV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(uarg{XV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{XV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("XV_CPTR") {
        CHECK_FALSE(uarg{XV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(uarg{XV_CPTR()}.can_cast_to<A* const>());
        CHECK(uarg{XV_CPTR()}.can_cast_to<const A*>());
        CHECK(uarg{XV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = uarg{XV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = uarg{XV_CPTR()}.cast<A* const>());
        CHECK(uarg{XV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{XV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CXV_PTR") {
        CHECK(uarg{CXV_PTR()}.can_cast_to<A*>());
        CHECK(uarg{CXV_PTR()}.can_cast_to<A* const>());
        CHECK(uarg{CXV_PTR()}.can_cast_to<const A*>());
        CHECK(uarg{CXV_PTR()}.can_cast_to<const A* const>());

        CHECK(uarg{CXV_PTR()}.cast<A*>()->f() == 1);
        CHECK(uarg{CXV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(uarg{CXV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{CXV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CXV_CPTR") {
        CHECK_FALSE(uarg{CXV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(uarg{CXV_CPTR()}.can_cast_to<A* const>());
        CHECK(uarg{CXV_CPTR()}.can_cast_to<const A*>());
        CHECK(uarg{CXV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = uarg{CXV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = uarg{CXV_CPTR()}.cast<A* const>());
        CHECK(uarg{CXV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{CXV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("PRV_PTR") {
        CHECK(uarg{PRV_PTR()}.can_cast_to<A*>());
        CHECK(uarg{PRV_PTR()}.can_cast_to<A* const>());
        CHECK(uarg{PRV_PTR()}.can_cast_to<const A*>());
        CHECK(uarg{PRV_PTR()}.can_cast_to<const A* const>());

        CHECK(uarg{PRV_PTR()}.cast<A*>()->f() == 1);
        CHECK(uarg{PRV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(uarg{PRV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{PRV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("PRV_CPTR") {
        CHECK_FALSE(uarg{PRV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(uarg{PRV_CPTR()}.can_cast_to<A* const>());
        CHECK(uarg{PRV_CPTR()}.can_cast_to<const A*>());
        CHECK(uarg{PRV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = uarg{PRV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = uarg{PRV_CPTR()}.cast<A* const>());
        CHECK(uarg{PRV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(uarg{PRV_CPTR()}.cast<const A* const>()->f() == 1);
    }
}
