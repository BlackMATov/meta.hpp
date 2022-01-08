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

        A(A&&) noexcept { ++move_ctors_; }
        A(const A&) { ++copy_ctors_; }

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
        int i = 2;
        [[maybe_unused, nodiscard]] int f() const { return i; }
    };

    struct C : virtual A {
        int i = 3;
        [[maybe_unused, nodiscard]] int f() const { return i; }
    };

    struct D : B, C {
        int i = 4;
        [[maybe_unused, nodiscard]] int f() const { return i; }
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
    using meta::detail::arg;

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
        CHECK(arg{LV()}.can_cast_to<A>());
        CHECK(arg{LV()}.can_cast_to<const A>());
        CHECK(arg{LV()}.can_cast_to<A&>());
        CHECK(arg{LV()}.can_cast_to<const A&>());
        CHECK_FALSE(arg{LV()}.can_cast_to<A&&>());
        CHECK_FALSE(arg{LV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(arg{LV()}.cast<A>().f() == 1);
        CHECK(arg{LV()}.cast<const A>().f() == 1);
        CHECK(arg{LV()}.cast<A&>().f() == 1);
        CHECK(arg{LV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = arg{LV()}.cast<A&&>());
        CHECK_THROWS(std::ignore = arg{LV()}.cast<const A&&>());

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("CLV") {
        CHECK(arg{CLV()}.can_cast_to<A>());
        CHECK(arg{CLV()}.can_cast_to<const A>());
        CHECK_FALSE(arg{CLV()}.can_cast_to<A&>());
        CHECK(arg{CLV()}.can_cast_to<const A&>());
        CHECK_FALSE(arg{CLV()}.can_cast_to<A&&>());
        CHECK_FALSE(arg{CLV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(arg{CLV()}.cast<A>().f() == 1);
        CHECK(arg{CLV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = arg{CLV()}.cast<A&>());
        CHECK(arg{CLV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = arg{CLV()}.cast<A&&>());
        CHECK_THROWS(std::ignore = arg{CLV()}.cast<const A&&>());

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("XV") {
        CHECK(arg{XV()}.can_cast_to<A>());
        CHECK(arg{XV()}.can_cast_to<const A>());
        CHECK_FALSE(arg{XV()}.can_cast_to<A&>());
        CHECK(arg{XV()}.can_cast_to<const A&>());
        CHECK(arg{XV()}.can_cast_to<A&&>());
        CHECK(arg{XV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(arg{XV()}.cast<A>().f() == 1);
        CHECK(arg{XV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = arg{XV()}.cast<A&>());
        CHECK(arg{XV()}.cast<const A&>().f() == 1);
        CHECK(arg{XV()}.cast<A&&>().f() == 1);
        CHECK(arg{XV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 2);
    }

    SUBCASE("CXV") {
        CHECK(arg{CXV()}.can_cast_to<A>());
        CHECK(arg{CXV()}.can_cast_to<const A>());
        CHECK_FALSE(arg{CXV()}.can_cast_to<A&>());
        CHECK(arg{CXV()}.can_cast_to<const A&>());
        CHECK_FALSE(arg{CXV()}.can_cast_to<A&&>());
        CHECK(arg{CXV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(arg{CXV()}.cast<A>().f() == 1);
        CHECK(arg{CXV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = arg{CXV()}.cast<A&>());
        CHECK(arg{CXV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = arg{CXV()}.cast<A&&>());
        CHECK(arg{CXV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("PRV") {
        CHECK(arg{PRV()}.can_cast_to<A>());
        CHECK(arg{PRV()}.can_cast_to<const A>());
        CHECK_FALSE(arg{PRV()}.can_cast_to<A&>());
        CHECK(arg{PRV()}.can_cast_to<const A&>());
        CHECK(arg{PRV()}.can_cast_to<A&&>());
        CHECK(arg{PRV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(arg{PRV()}.cast<A>().f() == 1);
        CHECK(arg{PRV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = arg{PRV()}.cast<A&>());
        CHECK(arg{PRV()}.cast<const A&>().f() == 1);
        CHECK(arg{PRV()}.cast<A&&>().f() == 1);
        CHECK(arg{PRV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 2);
    }

    SUBCASE("CPRV") {
        CHECK(arg{CPRV()}.can_cast_to<A>());
        CHECK(arg{CPRV()}.can_cast_to<const A>());
        CHECK_FALSE(arg{CPRV()}.can_cast_to<A&>());
        CHECK(arg{CPRV()}.can_cast_to<const A&>());
        CHECK_FALSE(arg{CPRV()}.can_cast_to<A&&>());
        CHECK(arg{CPRV()}.can_cast_to<const A&&>());

        CHECK(A::copy_ctors_ == 0);
        CHECK(A::move_ctors_ == 0);

        CHECK(arg{CPRV()}.cast<A>().f() == 1);
        CHECK(arg{CPRV()}.cast<const A>().f() == 1);
        CHECK_THROWS(std::ignore = arg{CPRV()}.cast<A&>());
        CHECK(arg{CPRV()}.cast<const A&>().f() == 1);
        CHECK_THROWS(std::ignore = arg{CPRV()}.cast<A&&>());
        CHECK(arg{CPRV()}.cast<const A&&>().f() == 1);

        CHECK(A::copy_ctors_ == 2);
        CHECK(A::move_ctors_ == 0);
    }

    SUBCASE("LV_PTR") {
        CHECK(arg{LV_PTR()}.can_cast_to<A*>());
        CHECK(arg{LV_PTR()}.can_cast_to<A* const>());
        CHECK(arg{LV_PTR()}.can_cast_to<const A*>());
        CHECK(arg{LV_PTR()}.can_cast_to<const A* const>());

        CHECK(arg{LV_PTR()}.cast<A*>()->f() == 1);
        CHECK(arg{LV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(arg{LV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{LV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("LV_CPTR") {
        CHECK_FALSE(arg{LV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(arg{LV_CPTR()}.can_cast_to<A* const>());
        CHECK(arg{LV_CPTR()}.can_cast_to<const A*>());
        CHECK(arg{LV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = arg{LV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = arg{LV_CPTR()}.cast<A* const>());
        CHECK(arg{LV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{LV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CLV_PTR") {
        CHECK(arg{CLV_PTR()}.can_cast_to<A*>());
        CHECK(arg{CLV_PTR()}.can_cast_to<A* const>());
        CHECK(arg{CLV_PTR()}.can_cast_to<const A*>());
        CHECK(arg{CLV_PTR()}.can_cast_to<const A* const>());

        CHECK(arg{CLV_PTR()}.cast<A*>()->f() == 1);
        CHECK(arg{CLV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(arg{CLV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{CLV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CLV_CPTR") {
        CHECK_FALSE(arg{CLV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(arg{CLV_CPTR()}.can_cast_to<A* const>());
        CHECK(arg{CLV_CPTR()}.can_cast_to<const A*>());
        CHECK(arg{CLV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = arg{CLV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = arg{CLV_CPTR()}.cast<A* const>());
        CHECK(arg{CLV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{CLV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("XV_PTR") {
        CHECK(arg{XV_PTR()}.can_cast_to<A*>());
        CHECK(arg{XV_PTR()}.can_cast_to<A* const>());
        CHECK(arg{XV_PTR()}.can_cast_to<const A*>());
        CHECK(arg{XV_PTR()}.can_cast_to<const A* const>());

        CHECK(arg{XV_PTR()}.cast<A*>()->f() == 1);
        CHECK(arg{XV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(arg{XV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{XV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("XV_CPTR") {
        CHECK_FALSE(arg{XV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(arg{XV_CPTR()}.can_cast_to<A* const>());
        CHECK(arg{XV_CPTR()}.can_cast_to<const A*>());
        CHECK(arg{XV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = arg{XV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = arg{XV_CPTR()}.cast<A* const>());
        CHECK(arg{XV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{XV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CXV_PTR") {
        CHECK(arg{CXV_PTR()}.can_cast_to<A*>());
        CHECK(arg{CXV_PTR()}.can_cast_to<A* const>());
        CHECK(arg{CXV_PTR()}.can_cast_to<const A*>());
        CHECK(arg{CXV_PTR()}.can_cast_to<const A* const>());

        CHECK(arg{CXV_PTR()}.cast<A*>()->f() == 1);
        CHECK(arg{CXV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(arg{CXV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{CXV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("CXV_CPTR") {
        CHECK_FALSE(arg{CXV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(arg{CXV_CPTR()}.can_cast_to<A* const>());
        CHECK(arg{CXV_CPTR()}.can_cast_to<const A*>());
        CHECK(arg{CXV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = arg{CXV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = arg{CXV_CPTR()}.cast<A* const>());
        CHECK(arg{CXV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{CXV_CPTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("PRV_PTR") {
        CHECK(arg{PRV_PTR()}.can_cast_to<A*>());
        CHECK(arg{PRV_PTR()}.can_cast_to<A* const>());
        CHECK(arg{PRV_PTR()}.can_cast_to<const A*>());
        CHECK(arg{PRV_PTR()}.can_cast_to<const A* const>());

        CHECK(arg{PRV_PTR()}.cast<A*>()->f() == 1);
        CHECK(arg{PRV_PTR()}.cast<A* const>()->f() == 1);
        CHECK(arg{PRV_PTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{PRV_PTR()}.cast<const A* const>()->f() == 1);
    }

    SUBCASE("PRV_CPTR") {
        CHECK_FALSE(arg{PRV_CPTR()}.can_cast_to<A*>());
        CHECK_FALSE(arg{PRV_CPTR()}.can_cast_to<A* const>());
        CHECK(arg{PRV_CPTR()}.can_cast_to<const A*>());
        CHECK(arg{PRV_CPTR()}.can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = arg{PRV_CPTR()}.cast<A*>());
        CHECK_THROWS(std::ignore = arg{PRV_CPTR()}.cast<A* const>());
        CHECK(arg{PRV_CPTR()}.cast<const A*>()->f() == 1);
        CHECK(arg{PRV_CPTR()}.cast<const A* const>()->f() == 1);
    }
}
