/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct clazz {
        [[maybe_unused]] clazz() = default;
        [[maybe_unused]] clazz(clazz&&) = default;
        [[maybe_unused]] clazz(const clazz&) = default;
        clazz& operator=(clazz&&) = delete;
        clazz& operator=(const clazz&) = delete;
    };

    struct dclazz : clazz {
        [[maybe_unused]] dclazz() = default;
        [[maybe_unused]] dclazz(dclazz&&) = default;
        [[maybe_unused]] dclazz(const dclazz&) = default;
        dclazz& operator=(dclazz&&) = delete;
        dclazz& operator=(const dclazz&) = delete;
    };

    int f1(clazz) { return 1; }
    int f2(const clazz) { return 1; }
    int f3(clazz&) { return 1; }
    int f4(const clazz&) { return 1; }
    int f5(clazz&&) { return 1; }
    int f6(const clazz&&) { return 1; }

    int f1(clazz*) { return 1; }
    int f2(clazz* const) { return 1; }
    // int f3(clazz*&) { return 1; }
    // int f4(clazz* const&) { return 1; }
    // int f5(clazz*&&) { return 1; }
    // int f6(clazz* const&&) { return 1; }

    int f1(const clazz*) { return 1; }
    int f2(const clazz* const) { return 1; }
    // int f3(const clazz*&) { return 1; }
    // int f4(const clazz* const&) { return 1; }
    // int f5(const clazz*&&) { return 1; }
    // int f6(const clazz* const&&) { return 1; }
}

#define META_HPP_CHECK_INVOCABLE(FromValue, FName, ToType)\
    {\
        using namespace meta::detail;\
        auto function_ptr = meta::select<int(ToType)>(&FName);\
        meta::function f_state{function_state::make("", function_ptr)};\
        \
        if ( std::is_invocable_v<decltype(function_ptr), decltype(FromValue)> ) {\
            CHECK(arg{FromValue}.can_cast_to<ToType>());\
            CHECK(arg_base{type_list<decltype(FromValue)>{}}.can_cast_to<ToType>());\
            CHECK_NOTHROW(arg{FromValue}.cast<ToType>());\
            \
            CHECK(f_state.is_invocable_with<decltype(FromValue)>());\
            CHECK(f_state.invoke(FromValue) == 1);\
        } else {\
            CHECK_FALSE(arg{FromValue}.can_cast_to<ToType>());\
            CHECK_FALSE(arg_base{type_list<decltype(FromValue)>{}}.can_cast_to<ToType>());\
            CHECK_THROWS(arg{FromValue}.cast<ToType>());\
            \
            CHECK_FALSE(f_state.is_invocable_with<decltype(FromValue)>());\
            CHECK_THROWS(f_state.invoke(FromValue));\
        }\
    }

#define META_HPP_CHECK_INVOCABLE_2(FromValue, FName, FromType, ToType)\
    {\
        using namespace meta::detail;\
        auto function_ptr = meta::select<int(ToType)>(&FName);\
        meta::function f_state{function_state::make("", function_ptr)};\
        \
        if ( std::is_invocable_v<decltype(function_ptr), FromType> ) {\
            CHECK(f_state.is_invocable_with<FromType>());\
            CHECK(f_state.is_invocable_with(FromValue));\
            CHECK(f_state.invoke(FromValue) == 1);\
        } else {\
            CHECK_FALSE(f_state.is_invocable_with<FromType>());\
            CHECK_FALSE(f_state.is_invocable_with(FromValue));\
            CHECK_THROWS(f_state.invoke(FromValue));\
        }\
    }

TEST_CASE("features/meta_utilities/arg2") {
    namespace meta = meta_hpp;

    meta::class_<dclazz>()
        .base_<clazz>();
}

TEST_CASE("features/meta_utilities/arg2/refs") {
    namespace meta = meta_hpp;

    {
        // lvalue
        auto LV = []() -> clazz& { static clazz v; return v; };
        auto LV2 = []() -> dclazz& { static dclazz v; return v; };

        meta::detail::arg a{LV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::lvalue);

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
    }

    {
        // const lvalue
        auto CLV = []() -> const clazz& { static clazz v; return v; };
        auto CLV2 = []() -> const dclazz& { static dclazz v; return v; };

        meta::detail::arg a{CLV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_lvalue);

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
    }

    {
        // xvalue
        auto XV = []() -> clazz&& { static clazz v; return std::move(v); };
        auto XV2 = []() -> dclazz&& { static dclazz v; return std::move(v); };

        meta::detail::arg a{XV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
    }

    {
        // const xvalue
        auto CXV = []() -> const clazz&& { static clazz v; return std::move(v); };
        auto CXV2 = []() -> const dclazz&& { static dclazz v; return std::move(v); };

        meta::detail::arg a{CXV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
    }

    {
        // prvalue
        auto PRV = []() -> clazz { return clazz{}; };
        auto PRV2 = []() -> dclazz { return dclazz{}; };

        meta::detail::arg a{PRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
    }

    {
        // const prvalue
        auto CPRV = []() -> const clazz { return clazz{}; };
        auto CPRV2 = []() -> const dclazz { return dclazz{}; };

        meta::detail::arg a{CPRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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

TEST_CASE("features/meta_utilities/arg2/ptrs") {
    namespace meta = meta_hpp;

    {
        // lvalue
        {
            auto LV_PTR = []() -> clazz*& { static clazz* p{}; return p; };
            auto LV2_PTR = []() -> dclazz*& { static dclazz* p{}; return p; };

            meta::detail::arg a{LV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::lvalue);

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
        }
        {
            auto LV_CPTR = []() -> const clazz*& { static const clazz* p{}; return p; };
            auto LV2_CPTR = []() -> const dclazz*& { static const dclazz* p{}; return p; };

            meta::detail::arg a{LV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::lvalue);

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
        }
    }

    {
        // const lvalue
        {
            auto CLV_PTR = []() -> clazz* const& { static clazz* p{}; return p; };
            auto CLV2_PTR = []() -> dclazz* const& { static dclazz* p{}; return p; };

            meta::detail::arg a{CLV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_lvalue);

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
        }
        {
            auto CLV_CPTR = []() -> const clazz* const& { static const clazz* p{}; return p; };
            auto CLV2_CPTR = []() -> const dclazz* const& { static const dclazz* p{}; return p; };

            meta::detail::arg a{CLV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_lvalue);

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
        }
    }

    {
        // xvalue
        {
            auto XV_PTR = []() -> clazz*&& { static clazz* p{}; return std::move(p); };
            auto XV2_PTR = []() -> dclazz*&& { static dclazz* p{}; return std::move(p); };

            meta::detail::arg a{XV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        }
        {
            auto XV_CPTR = []() -> const clazz*&& { static const clazz* p{}; return std::move(p); };
            auto XV2_CPTR = []() -> const dclazz*&& { static const dclazz* p{}; return std::move(p); };

            meta::detail::arg a{XV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        }
    }

    {
        // const xvalue
        {
            auto CXV_PTR = []() -> clazz* const&& { static clazz* p{}; return std::move(p); };
            auto CXV2_PTR = []() -> dclazz* const&& { static dclazz* p{}; return std::move(p); };

            meta::detail::arg a{CXV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
        }
        {
            auto CXV_CPTR = []() -> const clazz* const&& { static const clazz* p{}; return std::move(p); };
            auto CXV2_CPTR = []() -> const dclazz* const&& { static const dclazz* p{}; return std::move(p); };

            meta::detail::arg a{CXV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
        }
    }

    {
        // prvalue
        {
            auto PRV_PTR = []() -> clazz* { static clazz* p{}; return p; };
            auto PRV2_PTR = []() -> dclazz* { static dclazz* p{}; return p; };

            meta::detail::arg a{PRV_PTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        }
        {
            auto PRV_CPTR = []() -> const clazz* { static const clazz* p{}; return p; };
            auto PRV2_CPTR = []() -> const dclazz* { static const dclazz* p{}; return p; };

            meta::detail::arg a{PRV_CPTR()};
            CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
            CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        }
    }
}

TEST_CASE("features/meta_utilities/arg2/values") {
    namespace meta = meta_hpp;

    {
        auto LV = []() -> meta::value& { static meta::value v{clazz{}}; return v; };
        auto LV2 = []() -> meta::value& { static meta::value v{dclazz{}}; return v; };

        meta::detail::arg a{LV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::lvalue);

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
    }

    {
        auto CLV = []() -> const meta::value& { static meta::value v{clazz{}}; return v; };
        auto CLV2 = []() -> const meta::value& { static meta::value v{dclazz{}}; return v; };

        meta::detail::arg a{CLV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_lvalue);

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
    }

    {
        auto XV = []() -> meta::value&& { static meta::value v{clazz{}}; return std::move(v); };
        auto XV2 = []() -> meta::value&& { static meta::value v{dclazz{}}; return std::move(v); };

        meta::detail::arg a{XV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
    }

    {
        auto CXV = []() -> const meta::value&& { static meta::value v{clazz{}}; return std::move(v); };
        auto CXV2 = []() -> const meta::value&& { static meta::value v{dclazz{}}; return std::move(v); };

        meta::detail::arg a{CXV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
    }

    {
        auto PRV = []() -> meta::value { return meta::value{clazz{}}; };
        auto PRV2 = []() -> meta::value { return meta::value{dclazz{}}; };

        meta::detail::arg a{PRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
    }

    {
        auto CPRV = []() -> const meta::value { return meta::value{clazz{}}; };
        auto CPRV2 = []() -> const meta::value { return meta::value{dclazz{}}; };

        meta::detail::arg a{CPRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
    }
}

TEST_CASE("features/meta_utilities/arg2/ptr_values") {
    namespace meta = meta_hpp;

    {
        auto LV_PTR = []() -> meta::value& { static clazz* p{}; static meta::value v{p}; return v; };
        auto LV2_PTR = []() -> meta::value& { static dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{LV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::lvalue);

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
        auto LV_CPTR = []() -> meta::value& { static const clazz* p{}; static meta::value v{p}; return v; };
        auto LV2_CPTR = []() -> meta::value& { static const dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{LV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::lvalue);

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
        auto CLV_PTR = []() -> const meta::value& { static clazz* p{}; static meta::value v{p}; return v; };
        auto CLV2_PTR = []() -> const meta::value& { static dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{CLV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_lvalue);

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
        auto CLV_CPTR = []() -> const meta::value& { static const clazz* p{}; static meta::value v{p}; return v; };
        auto CLV2_CPTR = []() -> const meta::value& { static const dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{CLV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_lvalue);

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
        auto XV_PTR = []() -> meta::value&& { static clazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };
        auto XV2_PTR = []() -> meta::value&& { static dclazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };

        meta::detail::arg a{XV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        auto XV_CPTR = []() -> meta::value&& { static const clazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };
        auto XV2_CPTR = []() -> meta::value&& { static const dclazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };

        meta::detail::arg a{XV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        auto CXV_PTR = []() -> const meta::value&& { static clazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };
        auto CXV2_PTR = []() -> const meta::value&& { static dclazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };

        meta::detail::arg a{CXV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
        auto CXV_CPTR = []() -> const meta::value&& { static const clazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };
        auto CXV2_CPTR = []() -> const meta::value&& { static const dclazz* p{}; static meta::value v{std::move(p)}; return std::move(v); };

        meta::detail::arg a{CXV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
        auto PRV_PTR = []() -> meta::value { static clazz* p{}; static meta::value v{p}; return v; };
        auto PRV2_PTR = []() -> meta::value { static dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{PRV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        auto PRV_CPTR = []() -> meta::value { static const clazz* p{}; static meta::value v{p}; return v; };
        auto PRV2_CPTR = []() -> meta::value { static const dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{PRV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rvalue);

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
        auto CPRV_PTR = []() -> const meta::value { static clazz* p{}; static meta::value v{p}; return v; };
        auto CPRV2_PTR = []() -> const meta::value { static dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{CPRV_PTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
        auto CPRV_CPTR = []() -> const meta::value { static const clazz* p{}; static meta::value v{p}; return v; };
        auto CPRV2_CPTR = []() -> const meta::value { static const dclazz* p{}; static meta::value v{p}; return v; };

        meta::detail::arg a{CPRV_CPTR()};
        CHECK(a.get_raw_type() == meta::resolve_type<const clazz*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::const_rvalue);

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
