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
    };

    struct B : virtual A {
        int bi = 2;
    };

    struct C : virtual A {
        int ci = 3;
    };

    struct D : B, C {
        int di = 4;
    };
}

TEST_CASE("meta/meta_utilities/arg5") {
    namespace meta = meta_hpp;

    //  * <- B <- *
    // A           D
    //  * <- C <- *

    meta::class_<A>();
    meta::class_<B>().base_<A>();
    meta::class_<C>().base_<A>();
    meta::class_<D>().base_<B>().base_<C>();
}

TEST_CASE("meta/meta_utilities/arg5/cast") {
    namespace meta = meta_hpp;
    using meta::detail::arg;

    SUBCASE("int[2]") {
        int arr[2]{1,2};
        CHECK(arg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(std::is_invocable_v<void(int*), int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int*), int (&) [2]>);
        static_assert(std::is_invocable_v<void(int* const), int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int* const), int (&) [2]>);

        CHECK(arg(arr).can_cast_to<int*>());
        CHECK(arg(arr).can_cast_to<const int*>());
        CHECK(arg(arr).can_cast_to<int* const>());
        CHECK(arg(arr).can_cast_to<const int* const>());

        CHECK(arg(arr).cast<int*>() == static_cast<int*>(arr));
        CHECK(arg(arr).cast<const int*>() == static_cast<const int*>(arr));
        CHECK(arg(arr).cast<int* const>() == static_cast<int*>(arr));
        CHECK(arg(arr).cast<const int* const>() == static_cast<const int*>(arr));
    }

    SUBCASE("const int[2]") {
        const int arr[2]{1,2};
        CHECK(arg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(!std::is_invocable_v<void(int*), const int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int*), const int (&) [2]>);
        static_assert(!std::is_invocable_v<void(int* const), const int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int* const), const int (&) [2]>);

        CHECK_FALSE(arg(arr).can_cast_to<int*>());
        CHECK(arg(arr).can_cast_to<const int*>());
        CHECK_FALSE(arg(arr).can_cast_to<int* const>());
        CHECK(arg(arr).can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = arg(arr).cast<int*>());
        CHECK(arg(arr).cast<const int*>() == static_cast<const int*>(arr));
        CHECK_THROWS(std::ignore = arg(arr).cast<int* const>());
        CHECK(arg(arr).cast<const int* const>() == static_cast<const int*>(arr));
    }

    SUBCASE("D[2]") {
        D arr[2];
        CHECK(arg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(std::is_invocable_v<void(A*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(A* const), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A* const), D (&) [2]>);

        CHECK(arg(arr).can_cast_to<A*>());
        CHECK(arg(arr).can_cast_to<const A*>());
        CHECK(arg(arr).can_cast_to<A* const>());
        CHECK(arg(arr).can_cast_to<const A* const>());

        CHECK(arg(arr).cast<A*>() == static_cast<A*>(arr));
        CHECK(arg(arr).cast<const A*>() == static_cast<const A*>(arr));
        CHECK(arg(arr).cast<A* const>() == static_cast<A*>(arr));
        CHECK(arg(arr).cast<const A* const>() == static_cast<const A*>(arr));
    }

    SUBCASE("const D[2]") {
        const D arr[2];
        CHECK(arg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(!std::is_invocable_v<void(A*), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A*), const D (&) [2]>);
        static_assert(!std::is_invocable_v<void(A* const), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A* const), const D (&) [2]>);

        CHECK_FALSE(arg(arr).can_cast_to<A*>());
        CHECK(arg(arr).can_cast_to<const A*>());
        CHECK_FALSE(arg(arr).can_cast_to<A* const>());
        CHECK(arg(arr).can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = arg(arr).cast<A*>());
        CHECK(arg(arr).cast<const A*>() == static_cast<const A*>(arr));
        CHECK_THROWS(std::ignore = arg(arr).cast<A* const>());
        CHECK(arg(arr).cast<const A* const>() == static_cast<const A*>(arr));
    }

    SUBCASE("&") {
        using T = D[2];
        static T src{};

        {
            auto LV = []() -> T& { return src; };
            CHECK(arg{LV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(std::is_invocable_v<void(A*), decltype(LV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(LV())>);
            static_assert(std::is_invocable_v<void(A* const), decltype(LV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(LV())>);

            CHECK_NOTHROW([](A*){}(LV()));
            CHECK_NOTHROW([](const A*){}(LV()));
            CHECK_NOTHROW([](A* const){}(LV()));
            CHECK_NOTHROW([](const A* const){}(LV()));

            CHECK(arg(LV()).cast<A*>() == static_cast<A*>(src));
            CHECK(arg(LV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK(arg(LV()).cast<A* const>() == static_cast<A*>(src));
            CHECK(arg(LV()).cast<const A* const>() == static_cast<const A*>(src));
        }

        {
            auto CLV = []() -> const T& { return src; };
            CHECK(arg{CLV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(!std::is_invocable_v<void(A*), decltype(CLV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(CLV())>);
            static_assert(!std::is_invocable_v<void(A* const), decltype(CLV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(CLV())>);

            CHECK_THROWS(std::ignore = arg(CLV()).cast<A*>());
            CHECK(arg(CLV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK_THROWS(std::ignore = arg(CLV()).cast<A* const>());
            CHECK(arg(CLV()).cast<const A* const>() == static_cast<const A*>(src));
        }

        {
            auto XV = []() -> T&& { return std::move(src); };
            CHECK(arg{XV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(std::is_invocable_v<void(A*), decltype(XV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(XV())>);
            static_assert(std::is_invocable_v<void(A* const), decltype(XV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(XV())>);

            CHECK(arg(XV()).cast<A*>() == static_cast<A*>(src));
            CHECK(arg(XV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK(arg(XV()).cast<A* const>() == static_cast<A*>(src));
            CHECK(arg(XV()).cast<const A* const>() == static_cast<const A*>(src));
        }

        {
            auto CXV = []() -> const T&& { return std::move(src); };
            CHECK(arg{CXV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(!std::is_invocable_v<void(A*), decltype(CXV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(CXV())>);
            static_assert(!std::is_invocable_v<void(A* const), decltype(CXV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(CXV())>);

            CHECK_THROWS(std::ignore = arg(CXV()).cast<A*>());
            CHECK(arg(CXV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK_THROWS(std::ignore = arg(CXV()).cast<A* const>());
            CHECK(arg(CXV()).cast<const A* const>() == static_cast<const A*>(src));
        }
    }

    SUBCASE("*") {
        {
            static D arr[2]{};

            static_assert(std::is_invocable_v<void(D (&) [2]), D (&) [2]>);
            static_assert(std::is_invocable_v<void(const D (&) [2]), D (&) [2]>);
            static_assert(std::is_invocable_v<void(D (*) [2]), D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (*) [2]), D (*) [2]>);
            static_assert(std::is_invocable_v<void(D (* const) [2]), D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (* const) [2]), D (*) [2]>);

            CHECK(arg{arr}.can_cast_to<D (&) [2]>());
            CHECK(arg{arr}.can_cast_to<const D (&) [2]>());
            CHECK(arg{&arr}.can_cast_to<D (*) [2]>());
            CHECK(arg{&arr}.can_cast_to<const D (*) [2]>());
            CHECK(arg{&arr}.can_cast_to<D (* const) [2]>());
            CHECK(arg{&arr}.can_cast_to<const D (* const) [2]>());

            CHECK(&arg{arr}.cast<D (&) [2]>() == &arr);
            CHECK(&arg{arr}.cast<const D (&) [2]>() == &arr);
            CHECK(arg{&arr}.cast<D (*) [2]>() == &arr);
            CHECK(arg{&arr}.cast<const D (*) [2]>() == &arr);
            CHECK(arg{&arr}.cast<D (* const) [2]>() == &arr);
            CHECK(arg{&arr}.cast<const D (* const) [2]>() == &arr);
        }

        {
            static const D arr[2]{};

            static_assert(!std::is_invocable_v<void(D (&) [2]), const D (&) [2]>);
            static_assert(std::is_invocable_v<void(const D (&) [2]), const D (&) [2]>);
            static_assert(!std::is_invocable_v<void(D (*) [2]), const D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (*) [2]), const D (*) [2]>);
            static_assert(!std::is_invocable_v<void(D (* const) [2]), const D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (* const) [2]), const D (*) [2]>);

            CHECK_FALSE(arg{arr}.can_cast_to<D (&) [2]>());
            CHECK(arg{arr}.can_cast_to<const D (&) [2]>());
            CHECK_FALSE(arg{&arr}.can_cast_to<D (*) [2]>());
            CHECK(arg{&arr}.can_cast_to<const D (*) [2]>());
            CHECK_FALSE(arg{&arr}.can_cast_to<D (* const) [2]>());
            CHECK(arg{&arr}.can_cast_to<const D (* const) [2]>());

            CHECK_THROWS(std::ignore = &arg{arr}.cast<D (&) [2]>());
            CHECK(&arg{arr}.cast<const D (&) [2]>() == &arr);
            CHECK_THROWS(std::ignore = arg{&arr}.cast<D (*) [2]>());
            CHECK(arg{&arr}.cast<const D (*) [2]>() == &arr);
            CHECK_THROWS(std::ignore = arg{&arr}.cast<D (* const) [2]>());
            CHECK(arg{&arr}.cast<const D (* const) [2]>() == &arr);
        }
    }
}
