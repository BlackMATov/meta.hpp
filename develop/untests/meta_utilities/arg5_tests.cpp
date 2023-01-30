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
    using meta::detail::uarg;

    SUBCASE("int[2]") {
        int arr[2]{1,2};
        CHECK(uarg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(std::is_invocable_v<void(int*), int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int*), int (&) [2]>);
        static_assert(std::is_invocable_v<void(int* const), int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int* const), int (&) [2]>);

        CHECK(uarg(arr).can_cast_to<int*>());
        CHECK(uarg(arr).can_cast_to<const int*>());
        CHECK(uarg(arr).can_cast_to<int* const>());
        CHECK(uarg(arr).can_cast_to<const int* const>());

        CHECK(uarg(arr).cast<int*>() == static_cast<int*>(arr));
        CHECK(uarg(arr).cast<const int*>() == static_cast<const int*>(arr));
        CHECK(uarg(arr).cast<int* const>() == static_cast<int*>(arr));
        CHECK(uarg(arr).cast<const int* const>() == static_cast<const int*>(arr));
    }

    SUBCASE("const int[2]") {
        const int arr[2]{1,2};
        CHECK(uarg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(!std::is_invocable_v<void(int*), const int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int*), const int (&) [2]>);
        static_assert(!std::is_invocable_v<void(int* const), const int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int* const), const int (&) [2]>);

        CHECK_FALSE(uarg(arr).can_cast_to<int*>());
        CHECK(uarg(arr).can_cast_to<const int*>());
        CHECK_FALSE(uarg(arr).can_cast_to<int* const>());
        CHECK(uarg(arr).can_cast_to<const int* const>());

        CHECK_THROWS(std::ignore = uarg(arr).cast<int*>());
        CHECK(uarg(arr).cast<const int*>() == static_cast<const int*>(arr));
        CHECK_THROWS(std::ignore = uarg(arr).cast<int* const>());
        CHECK(uarg(arr).cast<const int* const>() == static_cast<const int*>(arr));
    }

    SUBCASE("D[2]") {
        D arr[2];
        CHECK(uarg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(std::is_invocable_v<void(A*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(A* const), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A* const), D (&) [2]>);

        CHECK(uarg(arr).can_cast_to<A*>());
        CHECK(uarg(arr).can_cast_to<const A*>());
        CHECK(uarg(arr).can_cast_to<A* const>());
        CHECK(uarg(arr).can_cast_to<const A* const>());

        CHECK(uarg(arr).cast<A*>() == static_cast<A*>(arr));
        CHECK(uarg(arr).cast<const A*>() == static_cast<const A*>(arr));
        CHECK(uarg(arr).cast<A* const>() == static_cast<A*>(arr));
        CHECK(uarg(arr).cast<const A* const>() == static_cast<const A*>(arr));
    }

    SUBCASE("const D[2]") {
        const D arr[2];
        CHECK(uarg(arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(!std::is_invocable_v<void(A*), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A*), const D (&) [2]>);
        static_assert(!std::is_invocable_v<void(A* const), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A* const), const D (&) [2]>);

        CHECK_FALSE(uarg(arr).can_cast_to<A*>());
        CHECK(uarg(arr).can_cast_to<const A*>());
        CHECK_FALSE(uarg(arr).can_cast_to<A* const>());
        CHECK(uarg(arr).can_cast_to<const A* const>());

        CHECK_THROWS(std::ignore = uarg(arr).cast<A*>());
        CHECK(uarg(arr).cast<const A*>() == static_cast<const A*>(arr));
        CHECK_THROWS(std::ignore = uarg(arr).cast<A* const>());
        CHECK(uarg(arr).cast<const A* const>() == static_cast<const A*>(arr));
    }

    SUBCASE("&") {
        using T = D[2];
        static T src{};

        {
            auto LV = []() -> T& { return src; };
            CHECK(uarg{LV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(std::is_invocable_v<void(A*), decltype(LV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(LV())>);
            static_assert(std::is_invocable_v<void(A* const), decltype(LV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(LV())>);

            [](A*){}(LV());
            [](const A*){}(LV());
            [](A* const){}(LV());
            [](const A* const){}(LV());

            CHECK(uarg(LV()).cast<A*>() == static_cast<A*>(src));
            CHECK(uarg(LV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK(uarg(LV()).cast<A* const>() == static_cast<A*>(src));
            CHECK(uarg(LV()).cast<const A* const>() == static_cast<const A*>(src));
        }

        {
            auto CLV = []() -> const T& { return src; };
            CHECK(uarg{CLV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(!std::is_invocable_v<void(A*), decltype(CLV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(CLV())>);
            static_assert(!std::is_invocable_v<void(A* const), decltype(CLV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(CLV())>);

            CHECK_THROWS(std::ignore = uarg(CLV()).cast<A*>());
            CHECK(uarg(CLV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK_THROWS(std::ignore = uarg(CLV()).cast<A* const>());
            CHECK(uarg(CLV()).cast<const A* const>() == static_cast<const A*>(src));
        }

        {
            auto XV = []() -> T&& { return std::move(src); };
            CHECK(uarg{XV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(std::is_invocable_v<void(A*), decltype(XV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(XV())>);
            static_assert(std::is_invocable_v<void(A* const), decltype(XV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(XV())>);

            CHECK(uarg(XV()).cast<A*>() == static_cast<A*>(src));
            CHECK(uarg(XV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK(uarg(XV()).cast<A* const>() == static_cast<A*>(src));
            CHECK(uarg(XV()).cast<const A* const>() == static_cast<const A*>(src));
        }

        {
            auto CXV = []() -> const T&& { return std::move(src); };
            CHECK(uarg{CXV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(!std::is_invocable_v<void(A*), decltype(CXV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(CXV())>);
            static_assert(!std::is_invocable_v<void(A* const), decltype(CXV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(CXV())>);

            CHECK_THROWS(std::ignore = uarg(CXV()).cast<A*>());
            CHECK(uarg(CXV()).cast<const A*>() == static_cast<const A*>(src));
            CHECK_THROWS(std::ignore = uarg(CXV()).cast<A* const>());
            CHECK(uarg(CXV()).cast<const A* const>() == static_cast<const A*>(src));
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

            CHECK(uarg{arr}.can_cast_to<D (&) [2]>());
            CHECK(uarg{arr}.can_cast_to<const D (&) [2]>());
            CHECK(uarg{&arr}.can_cast_to<D (*) [2]>());
            CHECK(uarg{&arr}.can_cast_to<const D (*) [2]>());
            CHECK(uarg{&arr}.can_cast_to<D (* const) [2]>());
            CHECK(uarg{&arr}.can_cast_to<const D (* const) [2]>());

            CHECK(&uarg{arr}.cast<D (&) [2]>() == &arr);
            CHECK(&uarg{arr}.cast<const D (&) [2]>() == &arr);
            CHECK(uarg{&arr}.cast<D (*) [2]>() == &arr);
            CHECK(uarg{&arr}.cast<const D (*) [2]>() == &arr);
            CHECK(uarg{&arr}.cast<D (* const) [2]>() == &arr);
            CHECK(uarg{&arr}.cast<const D (* const) [2]>() == &arr);
        }

        {
            static const D arr[2]{};

            static_assert(!std::is_invocable_v<void(D (&) [2]), const D (&) [2]>);
            static_assert(std::is_invocable_v<void(const D (&) [2]), const D (&) [2]>);
            static_assert(!std::is_invocable_v<void(D (*) [2]), const D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (*) [2]), const D (*) [2]>);
            static_assert(!std::is_invocable_v<void(D (* const) [2]), const D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (* const) [2]), const D (*) [2]>);

            CHECK_FALSE(uarg{arr}.can_cast_to<D (&) [2]>());
            CHECK(uarg{arr}.can_cast_to<const D (&) [2]>());
            CHECK_FALSE(uarg{&arr}.can_cast_to<D (*) [2]>());
            CHECK(uarg{&arr}.can_cast_to<const D (*) [2]>());
            CHECK_FALSE(uarg{&arr}.can_cast_to<D (* const) [2]>());
            CHECK(uarg{&arr}.can_cast_to<const D (* const) [2]>());

            CHECK_THROWS(std::ignore = &uarg{arr}.cast<D (&) [2]>());
            CHECK(&uarg{arr}.cast<const D (&) [2]>() == &arr);
            CHECK_THROWS(std::ignore = uarg{&arr}.cast<D (*) [2]>());
            CHECK(uarg{&arr}.cast<const D (*) [2]>() == &arr);
            CHECK_THROWS(std::ignore = uarg{&arr}.cast<D (* const) [2]>());
            CHECK(uarg{&arr}.cast<const D (* const) [2]>() == &arr);
        }
    }
}
