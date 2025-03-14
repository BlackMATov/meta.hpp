/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct fake {
        int i = 10;
    };

    struct clazz {
        int ii = 1;
        [[nodiscard]] int m1() { return ii; }
        [[nodiscard]] int m2() & { return ii; }
        [[nodiscard]] int m3() && { return ii; }
        [[nodiscard]] int m4() const { return ii; }
        [[nodiscard]] int m5() const & { return ii; }
        [[nodiscard]] int m6() const && { return ii; }
    };

    struct dclazz : fake, clazz {
        META_HPP_ENABLE_BASE_INFO(fake, clazz)
    };
}

// NOLINTNEXTLINE(*-macro-usage)
#define META_HPP_CHECK_INVOCABLE(Inst, FName, Qualifiers)\
    {\
        using namespace meta::detail;\
        auto method_ptr = meta::select_overload<int() Qualifiers>(&clazz::FName);\
        meta::method m_state{method_state::make<meta::method_policy::as_copy_t>("", method_ptr, {})};\
        \
        if ( std::is_invocable_v<decltype(method_ptr), decltype(Inst)> ) {\
            CHECK(uinst{r, Inst}.can_cast_to<clazz Qualifiers>(r));\
            CHECK(uinst_base{r, type_list<decltype(Inst)>{}}.can_cast_to<clazz Qualifiers>(r));\
            std::ignore = uinst{r, Inst}.cast<clazz Qualifiers>(r);\
            \
            CHECK(m_state.is_invocable_with<decltype(Inst)>());\
            CHECK(m_state.invoke(Inst).as<int>() == 1);\
            CHECK(m_state.try_invoke(Inst).get_value().as<int>() == 1);\
        } else {\
            CHECK_FALSE(uinst{r, Inst}.can_cast_to<clazz Qualifiers>(r));\
            CHECK_FALSE(uinst_base{r, type_list<decltype(Inst)>{}}.can_cast_to<clazz Qualifiers>(r));\
            /*CHECK_THROWS(std::ignore = uinst{r, Inst}.cast<clazz Qualifiers>(r));*/\
            \
            CHECK_FALSE(m_state.is_invocable_with<decltype(Inst)>());\
            CHECK_FALSE(m_state.try_invoke(Inst));\
        }\
    }

// NOLINTNEXTLINE(*-macro-usage)
#define META_HPP_CHECK_INVOCABLE_2(FromValue, FName, FromType, ToQualifiers)\
    {\
        using namespace meta::detail;\
        auto method_ptr = meta::select_overload<int() ToQualifiers>(&clazz::FName);\
        meta::method m_state{method_state::make<meta::method_policy::as_copy_t>("", method_ptr, {})};\
        \
        if ( std::is_invocable_v<decltype(method_ptr), FromType> ) {\
            CHECK(m_state.is_invocable_with<FromType>());\
            CHECK(m_state.is_invocable_with(FromValue));\
            CHECK(m_state.invoke(FromValue).as<int>() == 1);\
            CHECK(m_state.try_invoke(FromValue).get_value().as<int>() == 1);\
        } else {\
            CHECK_FALSE(m_state.is_invocable_with<FromType>());\
            CHECK_FALSE(m_state.is_invocable_with(FromValue));\
            CHECK_FALSE(m_state.try_invoke(FromValue));\
        }\
    }

TEST_CASE("meta/meta_utilities/inst2/refs") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::uinst;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    {
        // lvalue
        auto LV = []() -> clazz& { static clazz v; return v; };
        auto LV2 = []() -> dclazz& { static dclazz v; return v; };

        {
            uinst i{r, LV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == uinst::ref_types::lvalue);
        }

        META_HPP_CHECK_INVOCABLE(LV(), m1, )
        META_HPP_CHECK_INVOCABLE(LV(), m2, &)
        META_HPP_CHECK_INVOCABLE(LV(), m3, &&)
        META_HPP_CHECK_INVOCABLE(LV(), m4, const)
        META_HPP_CHECK_INVOCABLE(LV(), m5, const &)
        META_HPP_CHECK_INVOCABLE(LV(), m6, const &&)

        META_HPP_CHECK_INVOCABLE(LV2(), m1, )
        META_HPP_CHECK_INVOCABLE(LV2(), m2, &)
        META_HPP_CHECK_INVOCABLE(LV2(), m3, &&)
        META_HPP_CHECK_INVOCABLE(LV2(), m4, const)
        META_HPP_CHECK_INVOCABLE(LV2(), m5, const &)
        META_HPP_CHECK_INVOCABLE(LV2(), m6, const &&)
    }

    {
        // const lvalue
        auto CLV = []() -> const clazz& { static clazz v; return v; };
        auto CLV2 = []() -> const dclazz& { static dclazz v; return v; };

        {
            uinst i{r, CLV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == uinst::ref_types::const_lvalue);
        }

        META_HPP_CHECK_INVOCABLE(CLV(), m1, )
        META_HPP_CHECK_INVOCABLE(CLV(), m2, &)
        META_HPP_CHECK_INVOCABLE(CLV(), m3, &&)
        META_HPP_CHECK_INVOCABLE(CLV(), m4, const)
        META_HPP_CHECK_INVOCABLE(CLV(), m5, const &)
        META_HPP_CHECK_INVOCABLE(CLV(), m6, const &&)

        META_HPP_CHECK_INVOCABLE(CLV2(), m1, )
        META_HPP_CHECK_INVOCABLE(CLV2(), m2, &)
        META_HPP_CHECK_INVOCABLE(CLV2(), m3, &&)
        META_HPP_CHECK_INVOCABLE(CLV2(), m4, const)
        META_HPP_CHECK_INVOCABLE(CLV2(), m5, const &)
        META_HPP_CHECK_INVOCABLE(CLV2(), m6, const &&)
    }

    {
        // xvalue
        auto XV = []() -> clazz&& { static clazz v; return std::move(v); };
        auto XV2 = []() -> dclazz&& { static dclazz v; return std::move(v); };

        {
            uinst i{r, XV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == uinst::ref_types::rvalue);
        }

        META_HPP_CHECK_INVOCABLE(XV(), m1, )
        META_HPP_CHECK_INVOCABLE(XV(), m2, &)
        META_HPP_CHECK_INVOCABLE(XV(), m3, &&)
        META_HPP_CHECK_INVOCABLE(XV(), m4, const)
        META_HPP_CHECK_INVOCABLE(XV(), m5, const &)
        META_HPP_CHECK_INVOCABLE(XV(), m6, const &&)

        META_HPP_CHECK_INVOCABLE(XV2(), m1, )
        META_HPP_CHECK_INVOCABLE(XV2(), m2, &)
        META_HPP_CHECK_INVOCABLE(XV2(), m3, &&)
        META_HPP_CHECK_INVOCABLE(XV2(), m4, const)
        META_HPP_CHECK_INVOCABLE(XV2(), m5, const &)
        META_HPP_CHECK_INVOCABLE(XV2(), m6, const &&)
    }

    {
        // const xvalue
        auto CXV = []() -> const clazz&& { static clazz v; return std::move(v); };
        auto CXV2 = []() -> const dclazz&& { static dclazz v; return std::move(v); };

        {
            uinst i{r, CXV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == uinst::ref_types::const_rvalue);
        }

        META_HPP_CHECK_INVOCABLE(CXV(), m1, )
        META_HPP_CHECK_INVOCABLE(CXV(), m2, &)
        META_HPP_CHECK_INVOCABLE(CXV(), m3, &&)
        META_HPP_CHECK_INVOCABLE(CXV(), m4, const)
        META_HPP_CHECK_INVOCABLE(CXV(), m5, const &)
        META_HPP_CHECK_INVOCABLE(CXV(), m6, const &&)

        META_HPP_CHECK_INVOCABLE(CXV2(), m1, )
        META_HPP_CHECK_INVOCABLE(CXV2(), m2, &)
        META_HPP_CHECK_INVOCABLE(CXV2(), m3, &&)
        META_HPP_CHECK_INVOCABLE(CXV2(), m4, const)
        META_HPP_CHECK_INVOCABLE(CXV2(), m5, const &)
        META_HPP_CHECK_INVOCABLE(CXV2(), m6, const &&)
    }

    {
        // prvalue
        auto PRV = []() -> clazz { return clazz{}; };
        auto PRV2 = []() -> dclazz { return dclazz{}; };

        {
            uinst i{r, PRV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == uinst::ref_types::rvalue);
        }

        META_HPP_CHECK_INVOCABLE(PRV(), m1, )
        META_HPP_CHECK_INVOCABLE(PRV(), m2, &)
        META_HPP_CHECK_INVOCABLE(PRV(), m3, &&)
        META_HPP_CHECK_INVOCABLE(PRV(), m4, const)
        META_HPP_CHECK_INVOCABLE(PRV(), m5, const &)
        META_HPP_CHECK_INVOCABLE(PRV(), m6, const &&)

        META_HPP_CHECK_INVOCABLE(PRV2(), m1, )
        META_HPP_CHECK_INVOCABLE(PRV2(), m2, &)
        META_HPP_CHECK_INVOCABLE(PRV2(), m3, &&)
        META_HPP_CHECK_INVOCABLE(PRV2(), m4, const)
        META_HPP_CHECK_INVOCABLE(PRV2(), m5, const &)
        META_HPP_CHECK_INVOCABLE(PRV2(), m6, const &&)
    }

    {
        // const prvalue
        auto CPRV = []() -> const clazz { return clazz{}; };
        auto CPRV2 = []() -> const dclazz { return dclazz{}; };

        {
            uinst i{r, CPRV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == uinst::ref_types::const_rvalue);
        }

        META_HPP_CHECK_INVOCABLE(CPRV(), m1, )
        META_HPP_CHECK_INVOCABLE(CPRV(), m2, &)
        META_HPP_CHECK_INVOCABLE(CPRV(), m3, &&)
        META_HPP_CHECK_INVOCABLE(CPRV(), m4, const)
        META_HPP_CHECK_INVOCABLE(CPRV(), m5, const &)
        META_HPP_CHECK_INVOCABLE(CPRV(), m6, const &&)

        META_HPP_CHECK_INVOCABLE(CPRV2(), m1, )
        META_HPP_CHECK_INVOCABLE(CPRV2(), m2, &)
        META_HPP_CHECK_INVOCABLE(CPRV2(), m3, &&)
        META_HPP_CHECK_INVOCABLE(CPRV2(), m4, const)
        META_HPP_CHECK_INVOCABLE(CPRV2(), m5, const &)
        META_HPP_CHECK_INVOCABLE(CPRV2(), m6, const &&)
    }
}

TEST_CASE("meta/meta_utilities/inst2/values") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::uinst;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    {
        // lvalue
        auto LV = []() -> meta::uvalue& { static meta::uvalue v{clazz{}}; return v; };
        auto LV2 = []() -> meta::uvalue& { static meta::uvalue v{dclazz{}}; return v; };

        uarg a{r, LV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::lvalue);

        META_HPP_CHECK_INVOCABLE_2(LV(), m1, clazz&, )
        META_HPP_CHECK_INVOCABLE_2(LV(), m2, clazz&, &)
        META_HPP_CHECK_INVOCABLE_2(LV(), m3, clazz&, &&)
        META_HPP_CHECK_INVOCABLE_2(LV(), m4, clazz&, const)
        META_HPP_CHECK_INVOCABLE_2(LV(), m5, clazz&, const &)
        META_HPP_CHECK_INVOCABLE_2(LV(), m6, clazz&, const &&)

        META_HPP_CHECK_INVOCABLE_2(LV2(), m1, dclazz&, )
        META_HPP_CHECK_INVOCABLE_2(LV2(), m2, dclazz&, &)
        META_HPP_CHECK_INVOCABLE_2(LV2(), m3, dclazz&, &&)
        META_HPP_CHECK_INVOCABLE_2(LV2(), m4, dclazz&, const)
        META_HPP_CHECK_INVOCABLE_2(LV2(), m5, dclazz&, const &)
        META_HPP_CHECK_INVOCABLE_2(LV2(), m6, dclazz&, const &&)
    }

    {
        // const lvalue
        auto CLV = []() -> const meta::uvalue& { static meta::uvalue v{clazz{}}; return v; };
        auto CLV2 = []() -> const meta::uvalue& { static meta::uvalue v{dclazz{}}; return v; };

        uarg a{r, CLV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_lvalue);

        META_HPP_CHECK_INVOCABLE_2(CLV(), m1, const clazz&, )
        META_HPP_CHECK_INVOCABLE_2(CLV(), m2, const clazz&, &)
        META_HPP_CHECK_INVOCABLE_2(CLV(), m3, const clazz&, &&)
        META_HPP_CHECK_INVOCABLE_2(CLV(), m4, const clazz&, const)
        META_HPP_CHECK_INVOCABLE_2(CLV(), m5, const clazz&, const &)
        META_HPP_CHECK_INVOCABLE_2(CLV(), m6, const clazz&, const &&)

        META_HPP_CHECK_INVOCABLE_2(CLV2(), m1, const dclazz&, )
        META_HPP_CHECK_INVOCABLE_2(CLV2(), m2, const dclazz&, &)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), m3, const dclazz&, &&)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), m4, const dclazz&, const)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), m5, const dclazz&, const &)
        META_HPP_CHECK_INVOCABLE_2(CLV2(), m6, const dclazz&, const &&)
    }

    {
        // xvalue
        auto XV = []() -> meta::uvalue&& { static meta::uvalue v{clazz{}}; return std::move(v); };
        auto XV2 = []() -> meta::uvalue&& { static meta::uvalue v{dclazz{}}; return std::move(v); };

        uarg a{r, XV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(XV(), m1, clazz&&, )
        META_HPP_CHECK_INVOCABLE_2(XV(), m2, clazz&&, &)
        META_HPP_CHECK_INVOCABLE_2(XV(), m3, clazz&&, &&)
        META_HPP_CHECK_INVOCABLE_2(XV(), m4, clazz&&, const)
        META_HPP_CHECK_INVOCABLE_2(XV(), m5, clazz&&, const &)
        META_HPP_CHECK_INVOCABLE_2(XV(), m6, clazz&&, const &&)

        META_HPP_CHECK_INVOCABLE_2(XV2(), m1, dclazz&&, )
        META_HPP_CHECK_INVOCABLE_2(XV2(), m2, dclazz&&, &)
        META_HPP_CHECK_INVOCABLE_2(XV2(), m3, dclazz&&, &&)
        META_HPP_CHECK_INVOCABLE_2(XV2(), m4, dclazz&&, const)
        META_HPP_CHECK_INVOCABLE_2(XV2(), m5, dclazz&&, const &)
        META_HPP_CHECK_INVOCABLE_2(XV2(), m6, dclazz&&, const &&)
    }

    {
        // const xvalue
        auto CXV = []() -> const meta::uvalue&& { static meta::uvalue v{clazz{}}; return std::move(v); };
        auto CXV2 = []() -> const meta::uvalue&& { static meta::uvalue v{dclazz{}}; return std::move(v); };

        uarg a{r, CXV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CXV(), m1, const clazz&&, )
        META_HPP_CHECK_INVOCABLE_2(CXV(), m2, const clazz&&, &)
        META_HPP_CHECK_INVOCABLE_2(CXV(), m3, const clazz&&, &&)
        META_HPP_CHECK_INVOCABLE_2(CXV(), m4, const clazz&&, const)
        META_HPP_CHECK_INVOCABLE_2(CXV(), m5, const clazz&&, const &)
        META_HPP_CHECK_INVOCABLE_2(CXV(), m6, const clazz&&, const &&)

        META_HPP_CHECK_INVOCABLE_2(CXV2(), m1, const dclazz&&, )
        META_HPP_CHECK_INVOCABLE_2(CXV2(), m2, const dclazz&&, &)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), m3, const dclazz&&, &&)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), m4, const dclazz&&, const)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), m5, const dclazz&&, const &)
        META_HPP_CHECK_INVOCABLE_2(CXV2(), m6, const dclazz&&, const &&)
    }

    {
        // prvalue
        auto PRV = []() -> meta::uvalue { return meta::uvalue{clazz{}}; };
        auto PRV2 = []() -> meta::uvalue { return meta::uvalue{dclazz{}}; };

        uarg a{r, PRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::rvalue);

        META_HPP_CHECK_INVOCABLE_2(PRV(), m1, clazz, )
        META_HPP_CHECK_INVOCABLE_2(PRV(), m2, clazz, &)
        META_HPP_CHECK_INVOCABLE_2(PRV(), m3, clazz, &&)
        META_HPP_CHECK_INVOCABLE_2(PRV(), m4, clazz, const)
        META_HPP_CHECK_INVOCABLE_2(PRV(), m5, clazz, const &)
        META_HPP_CHECK_INVOCABLE_2(PRV(), m6, clazz, const &&)

        META_HPP_CHECK_INVOCABLE_2(PRV2(), m1, dclazz, )
        META_HPP_CHECK_INVOCABLE_2(PRV2(), m2, dclazz, &)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), m3, dclazz, &&)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), m4, dclazz, const)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), m5, dclazz, const &)
        META_HPP_CHECK_INVOCABLE_2(PRV2(), m6, dclazz, const &&)
    }

    {
        // const prvalue
        auto CPRV = []() -> const meta::uvalue { return meta::uvalue{clazz{}}; };
        auto CPRV2 = []() -> const meta::uvalue { return meta::uvalue{dclazz{}}; };

        uarg a{r, CPRV()};
        CHECK(a.get_raw_type() == meta::resolve_type<clazz>());
        CHECK(a.get_ref_type() == uarg::ref_types::const_rvalue);

        META_HPP_CHECK_INVOCABLE_2(CPRV(), m1, const clazz, )
        META_HPP_CHECK_INVOCABLE_2(CPRV(), m2, const clazz, &)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), m3, const clazz, &&)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), m4, const clazz, const)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), m5, const clazz, const &)
        META_HPP_CHECK_INVOCABLE_2(CPRV(), m6, const clazz, const &&)

        META_HPP_CHECK_INVOCABLE_2(CPRV2(), m1, const dclazz, )
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), m2, const dclazz, &)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), m3, const dclazz, &&)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), m4, const dclazz, const)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), m5, const dclazz, const &)
        META_HPP_CHECK_INVOCABLE_2(CPRV2(), m6, const dclazz, const &&)
    }
}
