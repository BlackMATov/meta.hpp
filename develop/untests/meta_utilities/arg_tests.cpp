/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct clazz;
    struct dclazz;

    struct fake {
        int i = 10;
    };

    struct clazz {
        int ii = 1;
    };

    struct dclazz : fake, clazz {
    };

    int f1(int v) { return v; }
    int f2(const int v) { return v; }
    int f3(int& v) { return v; }
    int f4(const int& v) { return v; }
    int f5(int&& v) { return v; }
    int f6(const int&& v) { return v; }

    int f1(clazz v) { return v.ii; }
    int f2(const clazz v) { return v.ii; }
    int f3(clazz& v) { return v.ii; }
    int f4(const clazz& v) { return v.ii; }
    int f5(clazz&& v) { return v.ii; }
    int f6(const clazz&& v) { return v.ii; }

    int f1(int* v) { return *v; }
    int f2(int* const v) { return *v; }
    // int f3(int*&) { return *v; }
    // int f4(int* const&) { return *v; }
    // int f5(int*&&) { return *v; }
    // int f6(int* const&&) { return *v; }

    int f1(clazz* v) { return v->ii; }
    int f2(clazz* const v) { return v->ii; }
    // int f3(clazz*&) { return v->ii; }
    // int f4(clazz* const&) { return v->ii; }
    // int f5(clazz*&&) { return v->ii; }
    // int f6(clazz* const&&) { return v->ii; }

    int f1(const int* v) { return *v; }
    int f2(const int* const v) { return *v; }
    // int f3(const int*&) { return *v; }
    // int f4(const int* const&) { return *v; }
    // int f5(const int*&&) { return *v; }
    // int f6(const int* const&&) { return *v; }

    int f1(const clazz* v) { return v->ii; }
    int f2(const clazz* const v) { return v->ii; }
    // int f3(const clazz*&) { return v->ii; }
    // int f4(const clazz* const&) { return v->ii; }
    // int f5(const clazz*&&) { return v->ii; }
    // int f6(const clazz* const&&) { return v->ii; }
}

// NOLINTNEXTLINE(*-macro-usage)
#define META_HPP_CHECK_INVOCABLE(FromValue, FName, ToType)\
    {\
        using namespace meta::detail;\
        auto function_ptr = meta::select_overload<int(ToType)>(&FName);\
        meta::function f_state{function_state::make<meta::function_policy::as_copy_t>("", function_ptr, {})};\
        \
        if ( std::is_invocable_v<decltype(function_ptr), decltype(FromValue)> ) {\
            CHECK(uarg{r, FromValue}.can_cast_to<ToType>(r));\
            CHECK(uarg_base{r, type_list<decltype(FromValue)>{}}.can_cast_to<ToType>(r));\
            std::ignore = uarg{r, FromValue}.cast<ToType>(r);\
            \
            CHECK(f_state.is_invocable_with<decltype(FromValue)>());\
            CHECK(f_state.invoke(FromValue).get_as<int>() == 1);\
        } else {\
            CHECK_FALSE(uarg{r, FromValue}.can_cast_to<ToType>(r));\
            CHECK_FALSE(uarg_base{r, type_list<decltype(FromValue)>{}}.can_cast_to<ToType>(r));\
            /*CHECK_THROWS(std::ignore = uarg{r, FromValue}.cast<ToType>(r));*/\
            \
            CHECK_FALSE(f_state.is_invocable_with<decltype(FromValue)>());\
            CHECK_FALSE(f_state.try_invoke(FromValue));\
        }\
    }

// NOLINTNEXTLINE(*-macro-usage)
#define META_HPP_CHECK_INVOCABLE_2(FromValue, FName, FromType, ToType)\
    {\
        using namespace meta::detail;\
        auto function_ptr = meta::select_overload<int(ToType)>(&FName);\
        meta::function f_state{function_state::make<meta::function_policy::as_copy_t>("", function_ptr, {})};\
        \
        if ( std::is_invocable_v<decltype(function_ptr), FromType> ) {\
            CHECK(f_state.is_invocable_with<FromType>());\
            CHECK(f_state.is_invocable_with(FromValue));\
            CHECK(f_state.invoke(FromValue).get_as<int>() == 1);\
        } else {\
            CHECK_FALSE(f_state.is_invocable_with<FromType>());\
            CHECK_FALSE(f_state.is_invocable_with(FromValue));\
            CHECK_FALSE(f_state.try_invoke(FromValue));\
        }\
    }

TEST_CASE("meta/meta_utilities/arg") {
    namespace meta = meta_hpp;

    meta::class_<fake>();
    meta::class_<clazz>();
    meta::class_<dclazz>().base_<fake>().base_<clazz>();
}

TEST_CASE("meta/meta_utilities/arg/refs") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    {
        // lvalue
        auto LV = []() -> clazz& { static clazz v; return v; };
        auto LV2 = []() -> dclazz& { static dclazz v; return v; };
        auto LV3 = []() -> int& { static int v{1}; return v; };

        uarg a{r, LV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::lvalue);

        META_HPP_CHECK_INVOCABLE(LV(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(LV(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(LV(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(LV(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(LV(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(LV(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(LV2(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(LV2(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(LV2(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(LV2(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(LV2(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(LV2(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(LV3(), f1, int)
        META_HPP_CHECK_INVOCABLE(LV3(), f2, const int)
        META_HPP_CHECK_INVOCABLE(LV3(), f3, int&)
        META_HPP_CHECK_INVOCABLE(LV3(), f4, const int&)
        META_HPP_CHECK_INVOCABLE(LV3(), f5, int&&)
        META_HPP_CHECK_INVOCABLE(LV3(), f6, const int&&)
    }

    {
        // const lvalue
        auto CLV = []() -> const clazz& { static clazz v; return v; };
        auto CLV2 = []() -> const dclazz& { static dclazz v; return v; };
        auto CLV3 = []() -> const int& { static int v{1}; return v; };

        uarg a{r, CLV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_lvalue);

        META_HPP_CHECK_INVOCABLE(CLV(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(CLV(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(CLV(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(CLV(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(CLV(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(CLV(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(CLV2(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(CLV2(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(CLV2(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(CLV2(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(CLV2(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(CLV2(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(CLV3(), f1, int)
        META_HPP_CHECK_INVOCABLE(CLV3(), f2, const int)
        META_HPP_CHECK_INVOCABLE(CLV3(), f3, int&)
        META_HPP_CHECK_INVOCABLE(CLV3(), f4, const int&)
        META_HPP_CHECK_INVOCABLE(CLV3(), f5, int&&)
        META_HPP_CHECK_INVOCABLE(CLV3(), f6, const int&&)
    }

    {
        // xvalue
        auto XV = []() -> clazz&& { static clazz v; return std::move(v); };
        auto XV2 = []() -> dclazz&& { static dclazz v; return std::move(v); };
        auto XV3 = []() -> int&& { static int v{1}; return std::move(v); };

        uarg a{r, XV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE(XV(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(XV(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(XV(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(XV(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(XV(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(XV(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(XV2(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(XV2(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(XV2(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(XV2(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(XV2(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(XV2(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(XV3(), f1, int)
        META_HPP_CHECK_INVOCABLE(XV3(), f2, const int)
        META_HPP_CHECK_INVOCABLE(XV3(), f3, int&)
        META_HPP_CHECK_INVOCABLE(XV3(), f4, const int&)
        META_HPP_CHECK_INVOCABLE(XV3(), f5, int&&)
        META_HPP_CHECK_INVOCABLE(XV3(), f6, const int&&)
    }

    {
        // const xvalue
        auto CXV = []() -> const clazz&& { static clazz v; return std::move(v); };
        auto CXV2 = []() -> const dclazz&& { static dclazz v; return std::move(v); };
        auto CXV3 = []() -> const int&& { static int v{1}; return std::move(v); };

        uarg a{r, CXV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE(CXV(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(CXV(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(CXV(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(CXV(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(CXV(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(CXV(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(CXV2(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(CXV2(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(CXV2(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(CXV2(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(CXV2(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(CXV2(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(CXV3(), f1, int)
        META_HPP_CHECK_INVOCABLE(CXV3(), f2, const int)
        META_HPP_CHECK_INVOCABLE(CXV3(), f3, int&)
        META_HPP_CHECK_INVOCABLE(CXV3(), f4, const int&)
        META_HPP_CHECK_INVOCABLE(CXV3(), f5, int&&)
        META_HPP_CHECK_INVOCABLE(CXV3(), f6, const int&&)
    }

    {
        // prvalue
        auto PRV = []() -> clazz { return clazz{}; };
        auto PRV2 = []() -> dclazz { return dclazz{}; };
        auto PRV3 = []() -> int { return int{1}; };

        uarg a{r, PRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE(PRV(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(PRV(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(PRV(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(PRV(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(PRV(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(PRV(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(PRV2(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(PRV2(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(PRV2(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(PRV2(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(PRV2(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(PRV2(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(PRV3(), f1, int)
        META_HPP_CHECK_INVOCABLE(PRV3(), f2, const int)
        META_HPP_CHECK_INVOCABLE(PRV3(), f3, int&)
        META_HPP_CHECK_INVOCABLE(PRV3(), f4, const int&)
        META_HPP_CHECK_INVOCABLE(PRV3(), f5, int&&)
        META_HPP_CHECK_INVOCABLE(PRV3(), f6, const int&&)
    }

    {
        // const prvalue
        auto CPRV = []() -> const clazz { return clazz{}; };
        auto CPRV2 = []() -> const dclazz { return dclazz{}; };

        uarg a{r, CPRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE(CPRV(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(CPRV(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(CPRV(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(CPRV(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(CPRV(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(CPRV(), f6, const clazz&&)

        META_HPP_CHECK_INVOCABLE(CPRV2(), f1, clazz)
        META_HPP_CHECK_INVOCABLE(CPRV2(), f2, const clazz)
        META_HPP_CHECK_INVOCABLE(CPRV2(), f3, clazz&)
        META_HPP_CHECK_INVOCABLE(CPRV2(), f4, const clazz&)
        META_HPP_CHECK_INVOCABLE(CPRV2(), f5, clazz&&)
        META_HPP_CHECK_INVOCABLE(CPRV2(), f6, const clazz&&)
    }
}

TEST_CASE("meta/meta_utilities/arg/ptrs") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    {
        // lvalue
        {
            auto LV_PTR = []() -> clazz*& { static clazz v; static clazz* p{&v}; return p; };
            auto LV2_PTR = []() -> dclazz*& { static dclazz v; static dclazz* p{&v}; return p; };
            auto LV3_PTR = []() -> int*& { static int v{1}; static int* p{&v}; return p; };

            uarg a{r, LV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::lvalue);

            META_HPP_CHECK_INVOCABLE(LV_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(LV_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(LV_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV2_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(LV2_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV2_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(LV2_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV2_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV3_PTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(LV3_PTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(LV3_PTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(LV3_PTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(LV3_PTR(), f6, const int* const&&)
        }
        {
            auto LV_CPTR = []() -> const clazz*& { static clazz v; static const clazz* p{&v}; return p; };
            auto LV2_CPTR = []() -> const dclazz*& { static dclazz v; static const dclazz* p{&v}; return p; };
            auto LV3_CPTR = []() -> const int*& { static int v{1}; static const int* p{&v}; return p; };

            uarg a{r, LV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::lvalue);

            META_HPP_CHECK_INVOCABLE(LV_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(LV_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(LV_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(LV2_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(LV3_CPTR(), f6, const int* const&&)
        }
    }

    {
        // const lvalue
        {
            auto CLV_PTR = []() -> clazz* const& { static clazz v; static clazz* p{&v}; return p; };
            auto CLV2_PTR = []() -> dclazz* const& { static dclazz v; static dclazz* p{&v}; return p; };
            auto CLV3_PTR = []() -> int* const& { static int v{1}; static int* p{&v}; return p; };

            uarg a{r, CLV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::const_lvalue);

            META_HPP_CHECK_INVOCABLE(CLV_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CLV_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CLV_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV2_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(CLV3_PTR(), f6, const int* const&&)
        }
        {
            auto CLV_CPTR = []() -> const clazz* const& { static clazz v; static const clazz* p{&v}; return p; };
            auto CLV2_CPTR = []() -> const dclazz* const& { static dclazz v; static const dclazz* p{&v}; return p; };
            auto CLV3_CPTR = []() -> const int* const& { static int v{1}; static const int* p{&v}; return p; };

            uarg a{r, CLV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::const_lvalue);

            META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CLV2_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(CLV3_CPTR(), f6, const int* const&&)
        }
    }

    {
        // xvalue
        {
            auto XV_PTR = []() -> clazz*&& { static clazz v; static clazz* p{&v}; return std::move(p); };
            auto XV2_PTR = []() -> dclazz*&& { static dclazz v; static dclazz* p{&v}; return std::move(p); };
            auto XV3_PTR = []() -> int*&& { static int v{1}; static int* p{&v}; return std::move(p); };

            uarg a{r, XV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

            META_HPP_CHECK_INVOCABLE(XV_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(XV_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(XV_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV2_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(XV2_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV2_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(XV2_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV2_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV3_PTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(XV3_PTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(XV3_PTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(XV3_PTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(XV3_PTR(), f6, const int* const&&)
        }
        {
            auto XV_CPTR = []() -> const clazz*&& { static clazz v; static const clazz* p{&v}; return std::move(p); };
            auto XV2_CPTR = []() -> const dclazz*&& { static dclazz v; static const dclazz* p{&v}; return std::move(p); };
            auto XV3_CPTR = []() -> const int*&& { static int v{1}; static const int* p{&v}; return std::move(p); };

            uarg a{r, XV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

            META_HPP_CHECK_INVOCABLE(XV_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(XV_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(XV_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(XV2_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(XV3_CPTR(), f6, const int* const&&)
        }
    }

    {
        // const xvalue
        {
            auto CXV_PTR = []() -> clazz* const&& { static clazz v; static clazz* p{&v}; return std::move(p); };
            auto CXV2_PTR = []() -> dclazz* const&& { static dclazz v; static dclazz* p{&v}; return std::move(p); };
            auto CXV3_PTR = []() -> int* const&& { static int v{1}; static int* p{&v}; return std::move(p); };

            uarg a{r, CXV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

            META_HPP_CHECK_INVOCABLE(CXV_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CXV_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CXV_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV2_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(CXV3_PTR(), f6, const int* const&&)
        }
        {
            auto CXV_CPTR = []() -> const clazz* const&& { static clazz v; static const clazz* p{&v}; return std::move(p); };
            auto CXV2_CPTR = []() -> const dclazz* const&& { static dclazz v; static const dclazz* p{&v}; return std::move(p); };
            auto CXV3_CPTR = []() -> const int* const&& { static int v{1}; static const int* p{&v}; return std::move(p); };

            uarg a{r, CXV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

            META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(CXV2_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(CXV3_CPTR(), f6, const int* const&&)
        }
    }

    {
        // prvalue
        {
            auto PRV_PTR = []() -> clazz* { static clazz v; static clazz* p{&v}; return p; };
            auto PRV2_PTR = []() -> dclazz* { static dclazz v; static dclazz* p{&v}; return p; };
            auto PRV3_PTR = []() -> int* { static int v{1}; static int* p{&v}; return p; };

            uarg a{r, PRV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

            META_HPP_CHECK_INVOCABLE(PRV_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(PRV_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(PRV_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV2_PTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(PRV3_PTR(), f6, const int* const&&)
        }
        {
            auto PRV_CPTR = []() -> const clazz* { static clazz v; static const clazz* p{&v}; return p; };
            auto PRV2_CPTR = []() -> const dclazz* { static dclazz v; static const dclazz* p{&v}; return p; };
            auto PRV3_CPTR = []() -> const int* { static int v{1}; static const int* p{&v}; return p; };

            uarg a{r, PRV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

            META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f1, clazz*)
            META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f2, clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f3, clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f4, clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f5, clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f6, clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f1, const clazz*)
            META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f2, const clazz* const)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f3, const clazz*&)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f4, const clazz* const&)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f5, const clazz*&&)
            // META_HPP_CHECK_INVOCABLE(PRV2_CPTR(), f6, const clazz* const&&)

            META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f1, int*)
            META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f2, int* const)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f3, int*&)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f4, int* const&)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f5, int*&&)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f6, int* const&&)

            META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f1, const int*)
            META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f2, const int* const)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f3, const int*&)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f4, const int* const&)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f5, const int*&&)
            // META_HPP_CHECK_INVOCABLE(PRV3_CPTR(), f6, const int* const&&)
        }
    }
}

TEST_CASE("meta/meta_utilities/arg/values") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    {
        auto LV = []() -> meta::uvalue& { static meta::uvalue v{clazz{}}; return v; };
        auto LV2 = []() -> meta::uvalue& { static meta::uvalue v{dclazz{}}; return v; };
        auto LV3 = []() -> meta::uvalue& { static meta::uvalue v{int{1}}; return v; };

        uarg a{r, LV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::lvalue);

        META_HPP_CHECK_INVOCABLE_2(LV(), f1, clazz&, clazz)
        META_HPP_CHECK_INVOCABLE_2(LV(), f2, clazz&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(LV(), f3, clazz&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(LV(), f4, clazz&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(LV(), f5, clazz&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(LV(), f6, clazz&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(LV2(), f1, dclazz&, clazz)
        META_HPP_CHECK_INVOCABLE_2(LV2(), f2, dclazz&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(LV2(), f3, dclazz&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(LV2(), f4, dclazz&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(LV2(), f5, dclazz&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(LV2(), f6, dclazz&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(LV3(), f1, int&, int)
        META_HPP_CHECK_INVOCABLE_2(LV3(), f2, int&, const int)
        META_HPP_CHECK_INVOCABLE_2(LV3(), f3, int&, int&)
        META_HPP_CHECK_INVOCABLE_2(LV3(), f4, int&, const int&)
        META_HPP_CHECK_INVOCABLE_2(LV3(), f5, int&, int&&)
        META_HPP_CHECK_INVOCABLE_2(LV3(), f6, int&, const int&&)
    }

    {
        auto CLV = []() -> const meta::uvalue& { static meta::uvalue v{clazz{}}; return v; };
        auto CLV2 = []() -> const meta::uvalue& { static meta::uvalue v{dclazz{}}; return v; };
        auto CLV3 = []() -> const meta::uvalue& { static meta::uvalue v{int{1}}; return v; };

        uarg a{r, CLV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_lvalue);

        META_HPP_CHECK_INVOCABLE_2(CLV(), f1, const clazz&, clazz)
        META_HPP_CHECK_INVOCABLE_2(CLV(), f2, const clazz&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(CLV(), f3, const clazz&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(CLV(), f4, const clazz&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(CLV(), f5, const clazz&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(CLV(), f6, const clazz&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(CLV2(), f1, const dclazz&, clazz)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), f2, const dclazz&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), f3, const dclazz&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), f4, const dclazz&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), f5, const dclazz&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), f6, const dclazz&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(CLV3(), f1, const int&, int)
        META_HPP_CHECK_INVOCABLE_2(CLV3(), f2, const int&, const int)
        META_HPP_CHECK_INVOCABLE_2(CLV3(), f3, const int&, int&)
        META_HPP_CHECK_INVOCABLE_2(CLV3(), f4, const int&, const int&)
        META_HPP_CHECK_INVOCABLE_2(CLV3(), f5, const int&, int&&)
        META_HPP_CHECK_INVOCABLE_2(CLV3(), f6, const int&, const int&&)
    }

    {
        auto XV = []() -> meta::uvalue&& { static meta::uvalue v{clazz{}}; return std::move(v); };
        auto XV2 = []() -> meta::uvalue&& { static meta::uvalue v{dclazz{}}; return std::move(v); };
        auto XV3 = []() -> meta::uvalue&& { static meta::uvalue v{int{1}}; return std::move(v); };

        uarg a{r, XV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(XV(), f1, clazz&&, clazz)
        META_HPP_CHECK_INVOCABLE_2(XV(), f2, clazz&&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(XV(), f3, clazz&&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(XV(), f4, clazz&&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(XV(), f5, clazz&&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(XV(), f6, clazz&&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(XV2(), f1, dclazz&&, clazz)
        META_HPP_CHECK_INVOCABLE_2(XV2(), f2, dclazz&&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(XV2(), f3, dclazz&&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(XV2(), f4, dclazz&&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(XV2(), f5, dclazz&&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(XV2(), f6, dclazz&&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(XV3(), f1, int&&, int)
        META_HPP_CHECK_INVOCABLE_2(XV3(), f2, int&&, const int)
        META_HPP_CHECK_INVOCABLE_2(XV3(), f3, int&&, int&)
        META_HPP_CHECK_INVOCABLE_2(XV3(), f4, int&&, const int&)
        META_HPP_CHECK_INVOCABLE_2(XV3(), f5, int&&, int&&)
        META_HPP_CHECK_INVOCABLE_2(XV3(), f6, int&&, const int&&)
    }

    {
        auto CXV = []() -> const meta::uvalue&& { static meta::uvalue v{clazz{}}; return std::move(v); };
        auto CXV2 = []() -> const meta::uvalue&& { static meta::uvalue v{dclazz{}}; return std::move(v); };
        auto CXV3 = []() -> const meta::uvalue&& { static meta::uvalue v{int{1}}; return std::move(v); };

        uarg a{r, CXV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CXV(), f1, const clazz&&, clazz)
        META_HPP_CHECK_INVOCABLE_2(CXV(), f2, const clazz&&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(CXV(), f3, const clazz&&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(CXV(), f4, const clazz&&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(CXV(), f5, const clazz&&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(CXV(), f6, const clazz&&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(CXV2(), f1, const dclazz&&, clazz)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), f2, const dclazz&&, const clazz)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), f3, const dclazz&&, clazz&)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), f4, const dclazz&&, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), f5, const dclazz&&, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), f6, const dclazz&&, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(CXV3(), f1, const int&&, int)
        META_HPP_CHECK_INVOCABLE_2(CXV3(), f2, const int&&, const int)
        META_HPP_CHECK_INVOCABLE_2(CXV3(), f3, const int&&, int&)
        META_HPP_CHECK_INVOCABLE_2(CXV3(), f4, const int&&, const int&)
        META_HPP_CHECK_INVOCABLE_2(CXV3(), f5, const int&&, int&&)
        META_HPP_CHECK_INVOCABLE_2(CXV3(), f6, const int&&, const int&&)
    }

    {
        auto PRV = []() -> meta::uvalue { return meta::uvalue{clazz{}}; };
        auto PRV2 = []() -> meta::uvalue { return meta::uvalue{dclazz{}}; };
        auto PRV3 = []() -> meta::uvalue { return meta::uvalue{int{1}}; };

        uarg a{r, PRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(PRV(), f1, clazz, clazz)
        META_HPP_CHECK_INVOCABLE_2(PRV(), f2, clazz, const clazz)
        META_HPP_CHECK_INVOCABLE_2(PRV(), f3, clazz, clazz&)
        META_HPP_CHECK_INVOCABLE_2(PRV(), f4, clazz, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(PRV(), f5, clazz, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(PRV(), f6, clazz, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(PRV2(), f1, dclazz, clazz)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), f2, dclazz, const clazz)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), f3, dclazz, clazz&)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), f4, dclazz, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), f5, dclazz, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), f6, dclazz, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(PRV3(), f1, int, int)
        META_HPP_CHECK_INVOCABLE_2(PRV3(), f2, int, const int)
        META_HPP_CHECK_INVOCABLE_2(PRV3(), f3, int, int&)
        META_HPP_CHECK_INVOCABLE_2(PRV3(), f4, int, const int&)
        META_HPP_CHECK_INVOCABLE_2(PRV3(), f5, int, int&&)
        META_HPP_CHECK_INVOCABLE_2(PRV3(), f6, int, const int&&)
    }

    {
        auto CPRV = []() -> const meta::uvalue { return meta::uvalue{clazz{}}; };
        auto CPRV2 = []() -> const meta::uvalue { return meta::uvalue{dclazz{}}; };
        auto CPRV3 = []() -> const meta::uvalue { return meta::uvalue{int{1}}; };

        uarg a{r, CPRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CPRV(), f1, const clazz, clazz)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), f2, const clazz, const clazz)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), f3, const clazz, clazz&)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), f4, const clazz, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), f5, const clazz, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), f6, const clazz, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV2(), f1, const dclazz, clazz)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), f2, const dclazz, const clazz)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), f3, const dclazz, clazz&)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), f4, const dclazz, const clazz&)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), f5, const dclazz, clazz&&)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), f6, const dclazz, const clazz&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV3(), f1, const int, int)
        META_HPP_CHECK_INVOCABLE_2(CPRV3(), f2, const int, const int)
        META_HPP_CHECK_INVOCABLE_2(CPRV3(), f3, const int, int&)
        META_HPP_CHECK_INVOCABLE_2(CPRV3(), f4, const int, const int&)
        META_HPP_CHECK_INVOCABLE_2(CPRV3(), f5, const int, int&&)
        META_HPP_CHECK_INVOCABLE_2(CPRV3(), f6, const int, const int&&)
    }
}

TEST_CASE("meta/meta_utilities/arg/ptr_values") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    {
        auto LV_PTR = []() -> meta::uvalue& { static clazz v; static clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto LV2_PTR = []() -> meta::uvalue& { static dclazz v; static dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, LV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::lvalue);

        META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f1, clazz*&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f2, clazz*&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f3, clazz*&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f4, clazz*&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f5, clazz*&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f6, clazz*&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f1, clazz*&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f2, clazz*&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f3, clazz*&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f4, clazz*&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f5, clazz*&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV_PTR(), f6, clazz*&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f1, dclazz*&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f2, dclazz*&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f3, dclazz*&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f4, dclazz*&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f5, dclazz*&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f6, dclazz*&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f1, dclazz*&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f2, dclazz*&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f3, dclazz*&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f4, dclazz*&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f5, dclazz*&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_PTR(), f6, dclazz*&, const clazz* const&&)
    }

    {
        auto LV_CPTR = []() -> meta::uvalue& { static clazz v; static const clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto LV2_CPTR = []() -> meta::uvalue& { static dclazz v; static const dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, LV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::lvalue);

        META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f1, const clazz*&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f2, const clazz*&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f3, const clazz*&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f4, const clazz*&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f5, const clazz*&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f6, const clazz*&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f1, const clazz*&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f2, const clazz*&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f3, const clazz*&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f4, const clazz*&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f5, const clazz*&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV_CPTR(), f6, const clazz*&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f1, const dclazz*&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f2, const dclazz*&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f3, const dclazz*&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f4, const dclazz*&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f5, const dclazz*&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f6, const dclazz*&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f1, const dclazz*&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f2, const dclazz*&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f3, const dclazz*&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f4, const dclazz*&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f5, const dclazz*&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(LV2_CPTR(), f6, const dclazz*&, const clazz* const&&)
    }

    {
        auto CLV_PTR = []() -> const meta::uvalue& { static clazz v; static clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto CLV2_PTR = []() -> const meta::uvalue& { static dclazz v; static dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, CLV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_lvalue);

        META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f1, clazz* const&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f2, clazz* const&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f3, clazz* const&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f4, clazz* const&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f5, clazz* const&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f6, clazz* const&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f1, clazz* const&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f2, clazz* const&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f3, clazz* const&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f4, clazz* const&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f5, clazz* const&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_PTR(), f6, clazz* const&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f1, dclazz* const&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f2, dclazz* const&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f3, dclazz* const&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f4, dclazz* const&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f5, dclazz* const&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f6, dclazz* const&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f1, dclazz* const&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f2, dclazz* const&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f3, dclazz* const&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f4, dclazz* const&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f5, dclazz* const&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_PTR(), f6, dclazz* const&, const clazz* const&&)
    }

    {
        auto CLV_CPTR = []() -> const meta::uvalue& { static clazz v; static const clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto CLV2_CPTR = []() -> const meta::uvalue& { static dclazz v; static const dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, CLV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_lvalue);

        META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f1, const clazz* const&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f2, const clazz* const&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f3, const clazz* const&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f4, const clazz* const&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f5, const clazz* const&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f6, const clazz* const&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f1, const clazz* const&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f2, const clazz* const&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f3, const clazz* const&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f4, const clazz* const&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f5, const clazz* const&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV_CPTR(), f6, const clazz* const&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f1, const dclazz* const&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f2, const dclazz* const&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f3, const dclazz* const&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f4, const dclazz* const&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f5, const dclazz* const&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f6, const dclazz* const&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f1, const dclazz* const&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f2, const dclazz* const&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f3, const dclazz* const&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f4, const dclazz* const&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f5, const dclazz* const&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CLV2_CPTR(), f6, const dclazz* const&, const clazz* const&&)
    }

    {
        auto XV_PTR = []() -> meta::uvalue&& { static clazz v; static clazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };
        auto XV2_PTR = []() -> meta::uvalue&& { static dclazz v; static dclazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };

        uarg a{r, XV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f1, clazz*&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f2, clazz*&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f3, clazz*&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f4, clazz*&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f5, clazz*&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f6, clazz*&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f1, clazz*&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f2, clazz*&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f3, clazz*&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f4, clazz*&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f5, clazz*&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV_PTR(), f6, clazz*&&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f1, dclazz*&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f2, dclazz*&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f3, dclazz*&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f4, dclazz*&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f5, dclazz*&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f6, dclazz*&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f1, dclazz*&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f2, dclazz*&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f3, dclazz*&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f4, dclazz*&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f5, dclazz*&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_PTR(), f6, dclazz*&&, const clazz* const&&)
    }

    {
        auto XV_CPTR = []() -> meta::uvalue&& { static clazz v; static const clazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };
        auto XV2_CPTR = []() -> meta::uvalue&& { static dclazz v; static const dclazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };

        uarg a{r, XV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f1, const clazz*&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f2, const clazz*&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f3, const clazz*&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f4, const clazz*&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f5, const clazz*&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f6, const clazz*&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f1, const clazz*&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f2, const clazz*&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f3, const clazz*&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f4, const clazz*&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f5, const clazz*&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV_CPTR(), f6, const clazz*&&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f1, const dclazz*&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f2, const dclazz*&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f3, const dclazz*&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f4, const dclazz*&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f5, const dclazz*&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f6, const dclazz*&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f1, const dclazz*&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f2, const dclazz*&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f3, const dclazz*&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f4, const dclazz*&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f5, const dclazz*&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(XV2_CPTR(), f6, const dclazz*&&, const clazz* const&&)
    }

    {
        auto CXV_PTR = []() -> const meta::uvalue&& { static clazz v; static clazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };
        auto CXV2_PTR = []() -> const meta::uvalue&& { static dclazz v; static dclazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };

        uarg a{r, CXV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f1, clazz* const&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f2, clazz* const&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f3, clazz* const&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f4, clazz* const&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f5, clazz* const&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f6, clazz* const&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f1, clazz* const&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f2, clazz* const&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f3, clazz* const&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f4, clazz* const&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f5, clazz* const&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_PTR(), f6, clazz* const&&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f1, dclazz* const&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f2, dclazz* const&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f3, dclazz* const&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f4, dclazz* const&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f5, dclazz* const&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f6, dclazz* const&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f1, dclazz* const&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f2, dclazz* const&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f3, dclazz* const&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f4, dclazz* const&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f5, dclazz* const&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_PTR(), f6, dclazz* const&&, const clazz* const&&)
    }

    {
        auto CXV_CPTR = []() -> const meta::uvalue&& { static clazz v; static const clazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };
        auto CXV2_CPTR = []() -> const meta::uvalue&& { static dclazz v; static const dclazz* p{&v}; static meta::uvalue vv{std::move(p)}; return std::move(vv); };

        uarg a{r, CXV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f1, const clazz* const&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f2, const clazz* const&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f3, const clazz* const&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f4, const clazz* const&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f5, const clazz* const&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f6, const clazz* const&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f1, const clazz* const&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f2, const clazz* const&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f3, const clazz* const&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f4, const clazz* const&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f5, const clazz* const&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV_CPTR(), f6, const clazz* const&&, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f1, const dclazz* const&&, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f2, const dclazz* const&&, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f3, const dclazz* const&&, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f4, const dclazz* const&&, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f5, const dclazz* const&&, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f6, const dclazz* const&&, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f1, const dclazz* const&&, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f2, const dclazz* const&&, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f3, const dclazz* const&&, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f4, const dclazz* const&&, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f5, const dclazz* const&&, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CXV2_CPTR(), f6, const dclazz* const&&, const clazz* const&&)
    }

    {
        auto PRV_PTR = []() -> meta::uvalue { static clazz v; static clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto PRV2_PTR = []() -> meta::uvalue { static dclazz v; static dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, PRV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f1, clazz*, clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f2, clazz*, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f3, clazz*, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f4, clazz*, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f5, clazz*, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f6, clazz*, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f1, clazz*, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f2, clazz*, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f3, clazz*, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f4, clazz*, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f5, clazz*, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_PTR(), f6, clazz*, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f1, dclazz*, clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f2, dclazz*, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f3, dclazz*, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f4, dclazz*, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f5, dclazz*, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f6, dclazz*, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f1, dclazz*, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f2, dclazz*, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f3, dclazz*, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f4, dclazz*, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f5, dclazz*, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_PTR(), f6, dclazz*, const clazz* const&&)
    }

    {
        auto PRV_CPTR = []() -> meta::uvalue { static clazz v; static const clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto PRV2_CPTR = []() -> meta::uvalue { static dclazz v; static const dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, PRV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f1, const clazz*, clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f2, const clazz*, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f3, const clazz*, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f4, const clazz*, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f5, const clazz*, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f6, const clazz*, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f1, const clazz*, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f2, const clazz*, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f3, const clazz*, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f4, const clazz*, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f5, const clazz*, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV_CPTR(), f6, const clazz*, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f1, const dclazz*, clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f2, const dclazz*, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f3, const dclazz*, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f4, const dclazz*, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f5, const dclazz*, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f6, const dclazz*, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f1, const dclazz*, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f2, const dclazz*, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f3, const dclazz*, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f4, const dclazz*, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f5, const dclazz*, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(PRV2_CPTR(), f6, const dclazz*, const clazz* const&&)
    }

    {
        auto CPRV_PTR = []() -> const meta::uvalue { static clazz v; static clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto CPRV2_PTR = []() -> const meta::uvalue { static dclazz v; static dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, CPRV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f1, clazz* const, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f2, clazz* const, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f3, clazz* const, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f4, clazz* const, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f5, clazz* const, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f6, clazz* const, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f1, clazz* const, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f2, clazz* const, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f3, clazz* const, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f4, clazz* const, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f5, clazz* const, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_PTR(), f6, clazz* const, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f1, dclazz* const, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f2, dclazz* const, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f3, dclazz* const, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f4, dclazz* const, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f5, dclazz* const, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f6, dclazz* const, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f1, dclazz* const, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f2, dclazz* const, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f3, dclazz* const, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f4, dclazz* const, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f5, dclazz* const, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_PTR(), f6, dclazz* const, const clazz* const&&)
    }

    {
        auto CPRV_CPTR = []() -> const meta::uvalue { static clazz v; static const clazz* p{&v}; static meta::uvalue vv{p}; return vv; };
        auto CPRV2_CPTR = []() -> const meta::uvalue { static dclazz v; static const dclazz* p{&v}; static meta::uvalue vv{p}; return vv; };

        uarg a{r, CPRV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f1, const clazz* const, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f2, const clazz* const, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f3, const clazz* const, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f4, const clazz* const, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f5, const clazz* const, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f6, const clazz* const, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f1, const clazz* const, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f2, const clazz* const, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f3, const clazz* const, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f4, const clazz* const, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f5, const clazz* const, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV_CPTR(), f6, const clazz* const, const clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f1, const dclazz* const, clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f2, const dclazz* const, clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f3, const dclazz* const, clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f4, const dclazz* const, clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f5, const dclazz* const, clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f6, const dclazz* const, clazz* const&&)

        META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f1, const dclazz* const, const clazz*)
        META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f2, const dclazz* const, const clazz* const)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f3, const dclazz* const, const clazz*&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f4, const dclazz* const, const clazz* const&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f5, const dclazz* const, const clazz*&&)
        // META_HPP_CHECK_INVOCABLE_2(CPRV2_CPTR(), f6, const dclazz* const, const clazz* const&&)
    }
}
