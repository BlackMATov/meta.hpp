/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
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
        META_HPP_ENABLE_BASE_INFO(A)
    };

    struct C : virtual A {
        int ci = 3;
        META_HPP_ENABLE_BASE_INFO(A)
    };

    struct D : B, C {
        int di = 4;
        META_HPP_ENABLE_BASE_INFO(B, C)
    };
}

TEST_CASE("meta/meta_utilities/arg5") {
    namespace meta = meta_hpp;

    //  * <- B <- *
    // A           D
    //  * <- C <- *
}

TEST_CASE("meta/meta_utilities/arg5/cast") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    SUBCASE("int[2]") {
        int arr[2]{1,2};
        CHECK(uarg(r, arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(std::is_invocable_v<void(int*), int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int*), int (&) [2]>);
        static_assert(std::is_invocable_v<void(int* const), int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int* const), int (&) [2]>);

        CHECK(uarg(r, arr).can_cast_to<int*>(r));
        CHECK(uarg(r, arr).can_cast_to<const int*>(r));
        CHECK(uarg(r, arr).can_cast_to<int* const>(r));
        CHECK(uarg(r, arr).can_cast_to<const int* const>(r));

        CHECK(uarg(r, arr).cast<int*>(r) == static_cast<int*>(arr));
        CHECK(uarg(r, arr).cast<const int*>(r) == static_cast<const int*>(arr));
        CHECK(uarg(r, arr).cast<int* const>(r) == static_cast<int*>(arr));
        CHECK(uarg(r, arr).cast<const int* const>(r) == static_cast<const int*>(arr));
    }

    SUBCASE("const int[2]") {
        const int arr[2]{1,2};
        CHECK(uarg(r, arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(!std::is_invocable_v<void(int*), const int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int*), const int (&) [2]>);
        static_assert(!std::is_invocable_v<void(int* const), const int (&) [2]>);
        static_assert(std::is_invocable_v<void(const int* const), const int (&) [2]>);

        CHECK_FALSE(uarg(r, arr).can_cast_to<int*>(r));
        CHECK(uarg(r, arr).can_cast_to<const int*>(r));
        CHECK_FALSE(uarg(r, arr).can_cast_to<int* const>(r));
        CHECK(uarg(r, arr).can_cast_to<const int* const>(r));

        // CHECK_THROWS(std::ignore = uarg(r, arr).cast<int*>(r));
        CHECK(uarg(r, arr).cast<const int*>(r) == static_cast<const int*>(arr));
        // CHECK_THROWS(std::ignore = uarg(r, arr).cast<int* const>(r));
        CHECK(uarg(r, arr).cast<const int* const>(r) == static_cast<const int*>(arr));
    }

    SUBCASE("D[2]") {
        D arr[2];
        CHECK(uarg(r, arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(std::is_invocable_v<void(A*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(A* const), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A* const), D (&) [2]>);

        CHECK(uarg(r, arr).can_cast_to<A*>(r));
        CHECK(uarg(r, arr).can_cast_to<const A*>(r));
        CHECK(uarg(r, arr).can_cast_to<A* const>(r));
        CHECK(uarg(r, arr).can_cast_to<const A* const>(r));

        CHECK(uarg(r, arr).cast<A*>(r) == static_cast<A*>(arr));
        CHECK(uarg(r, arr).cast<const A*>(r) == static_cast<const A*>(arr));
        CHECK(uarg(r, arr).cast<A* const>(r) == static_cast<A*>(arr));
        CHECK(uarg(r, arr).cast<const A* const>(r) == static_cast<const A*>(arr));
    }

    SUBCASE("const D[2]") {
        const D arr[2];
        CHECK(uarg(r, arr).get_raw_type() == meta::resolve_type(arr));

        static_assert(!std::is_invocable_v<void(A*), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A*), const D (&) [2]>);
        static_assert(!std::is_invocable_v<void(A* const), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const A* const), const D (&) [2]>);

        CHECK_FALSE(uarg(r, arr).can_cast_to<A*>(r));
        CHECK(uarg(r, arr).can_cast_to<const A*>(r));
        CHECK_FALSE(uarg(r, arr).can_cast_to<A* const>(r));
        CHECK(uarg(r, arr).can_cast_to<const A* const>(r));

        // CHECK_THROWS(std::ignore = uarg(r, arr).cast<A*>(r));
        CHECK(uarg(r, arr).cast<const A*>(r) == static_cast<const A*>(arr));
        // CHECK_THROWS(std::ignore = uarg(r, arr).cast<A* const>(r));
        CHECK(uarg(r, arr).cast<const A* const>(r) == static_cast<const A*>(arr));
    }

    SUBCASE("&") {
        using T = D[2];
        static T src{};

        {
            auto LV = []() -> T& { return src; };
            CHECK(uarg{r, LV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(std::is_invocable_v<void(A*), decltype(LV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(LV())>);
            static_assert(std::is_invocable_v<void(A* const), decltype(LV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(LV())>);

            [](A*){}(LV());
            [](const A*){}(LV());
            [](A* const){}(LV());
            [](const A* const){}(LV());

            CHECK(uarg(r, LV()).cast<A*>(r) == static_cast<A*>(src));
            CHECK(uarg(r, LV()).cast<const A*>(r) == static_cast<const A*>(src));
            CHECK(uarg(r, LV()).cast<A* const>(r) == static_cast<A*>(src));
            CHECK(uarg(r, LV()).cast<const A* const>(r) == static_cast<const A*>(src));
        }

        {
            auto CLV = []() -> const T& { return src; };
            CHECK(uarg{r, CLV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(!std::is_invocable_v<void(A*), decltype(CLV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(CLV())>);
            static_assert(!std::is_invocable_v<void(A* const), decltype(CLV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(CLV())>);

            // CHECK_THROWS(std::ignore = uarg(r, CLV()).cast<A*>(r));
            CHECK(uarg(r, CLV()).cast<const A*>(r) == static_cast<const A*>(src));
            // CHECK_THROWS(std::ignore = uarg(r, CLV()).cast<A* const>(r));
            CHECK(uarg(r, CLV()).cast<const A* const>(r) == static_cast<const A*>(src));
        }

        {
            auto XV = []() -> T&& { return std::move(src); };
            CHECK(uarg{r, XV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(std::is_invocable_v<void(A*), decltype(XV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(XV())>);
            static_assert(std::is_invocable_v<void(A* const), decltype(XV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(XV())>);

            CHECK(uarg(r, XV()).cast<A*>(r) == static_cast<A*>(src));
            CHECK(uarg(r, XV()).cast<const A*>(r) == static_cast<const A*>(src));
            CHECK(uarg(r, XV()).cast<A* const>(r) == static_cast<A*>(src));
            CHECK(uarg(r, XV()).cast<const A* const>(r) == static_cast<const A*>(src));
        }

        {
            auto CXV = []() -> const T&& { return std::move(src); };
            CHECK(uarg{r, CXV()}.get_raw_type() == meta::resolve_type<D[2]>());

            static_assert(!std::is_invocable_v<void(A*), decltype(CXV())>);
            static_assert(std::is_invocable_v<void(const A*), decltype(CXV())>);
            static_assert(!std::is_invocable_v<void(A* const), decltype(CXV())>);
            static_assert(std::is_invocable_v<void(const A* const), decltype(CXV())>);

            // CHECK_THROWS(std::ignore = uarg(r, CXV()).cast<A*>(r));
            CHECK(uarg(r, CXV()).cast<const A*>(r) == static_cast<const A*>(src));
            // CHECK_THROWS(std::ignore = uarg(r, CXV()).cast<A* const>(r));
            CHECK(uarg(r, CXV()).cast<const A* const>(r) == static_cast<const A*>(src));
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

            CHECK(uarg{r, arr}.can_cast_to<D (&) [2]>(r));
            CHECK(uarg{r, arr}.can_cast_to<const D (&) [2]>(r));
            CHECK(uarg{r, &arr}.can_cast_to<D (*) [2]>(r));
            CHECK(uarg{r, &arr}.can_cast_to<const D (*) [2]>(r));
            CHECK(uarg{r, &arr}.can_cast_to<D (* const) [2]>(r));
            CHECK(uarg{r, &arr}.can_cast_to<const D (* const) [2]>(r));

            CHECK(&uarg{r, arr}.cast<D (&) [2]>(r) == &arr);
            CHECK(&uarg{r, arr}.cast<const D (&) [2]>(r) == &arr);
            CHECK(uarg{r, &arr}.cast<D (*) [2]>(r) == &arr);
            CHECK(uarg{r, &arr}.cast<const D (*) [2]>(r) == &arr);
            CHECK(uarg{r, &arr}.cast<D (* const) [2]>(r) == &arr);
            CHECK(uarg{r, &arr}.cast<const D (* const) [2]>(r) == &arr);
        }

        {
            static const D arr[2]{};

            static_assert(!std::is_invocable_v<void(D (&) [2]), const D (&) [2]>);
            static_assert(std::is_invocable_v<void(const D (&) [2]), const D (&) [2]>);
            static_assert(!std::is_invocable_v<void(D (*) [2]), const D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (*) [2]), const D (*) [2]>);
            static_assert(!std::is_invocable_v<void(D (* const) [2]), const D (*) [2]>);
            static_assert(std::is_invocable_v<void(const D (* const) [2]), const D (*) [2]>);

            CHECK_FALSE(uarg{r, arr}.can_cast_to<D (&) [2]>(r));
            CHECK(uarg{r, arr}.can_cast_to<const D (&) [2]>(r));
            CHECK_FALSE(uarg{r, &arr}.can_cast_to<D (*) [2]>(r));
            CHECK(uarg{r, &arr}.can_cast_to<const D (*) [2]>(r));
            CHECK_FALSE(uarg{r, &arr}.can_cast_to<D (* const) [2]>(r));
            CHECK(uarg{r, &arr}.can_cast_to<const D (* const) [2]>(r));

            // CHECK_THROWS(std::ignore = &uarg{r, arr}.cast<D (&) [2]>(r));
            CHECK(&uarg{r, arr}.cast<const D (&) [2]>(r) == &arr);
            // CHECK_THROWS(std::ignore = uarg{r, &arr}.cast<D (*) [2]>(r));
            CHECK(uarg{r, &arr}.cast<const D (*) [2]>(r) == &arr);
            // CHECK_THROWS(std::ignore = uarg{r, &arr}.cast<D (* const) [2]>(r));
            CHECK(uarg{r, &arr}.cast<const D (* const) [2]>(r) == &arr);
        }
    }
}
