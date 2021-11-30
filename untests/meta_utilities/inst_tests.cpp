/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct clazz {
        int m1() { return 1; }
        int m2() & { return 1; }
        int m3() && { return 1; }
        int m4() const { return 1; }
        int m5() const & { return 1; }
        int m6() const && { return 1; }
    };

    struct dclazz : clazz {};
}

#define META_HPP_CHECK_INVOCABLE(Inst, FName, Qualifiers)\
    {\
        using namespace meta::detail;\
        auto method_ptr = meta::select<int() Qualifiers>(&clazz::FName);\
        meta::method m_state{method_state::make("", method_ptr)};\
        \
        if ( std::is_invocable_v<decltype(method_ptr), decltype(Inst)> ) {\
            CHECK(inst{Inst}.can_cast_to<clazz Qualifiers>());\
            CHECK(inst_base{type_list<decltype(Inst)>{}}.can_cast_to<clazz Qualifiers>());\
            \
            CHECK(m_state.is_invocable_with<decltype(Inst)>());\
            CHECK(m_state.invoke(Inst) == 1);\
        } else {\
            CHECK_FALSE(inst{Inst}.can_cast_to<clazz Qualifiers>());\
            CHECK_FALSE(inst_base{type_list<decltype(Inst)>{}}.can_cast_to<clazz Qualifiers>());\
            \
            CHECK_FALSE(m_state.is_invocable_with<decltype(Inst)>());\
            CHECK_THROWS(m_state.invoke(Inst));\
        }\
    }

TEST_CASE("features/meta_utilities/inst2") {
    namespace meta = meta_hpp;

    meta::class_<dclazz>()
        .base_<clazz>();
}

TEST_CASE("features/meta_utilities/inst2") {
    namespace meta = meta_hpp;

    {
        // lvalue
        auto LV = []() -> clazz& { static clazz v; return v; };
        auto LV2 = []() -> dclazz& { static dclazz v; return v; };

        {
            meta::detail::inst i{LV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == meta::detail::inst::ref_types::lvalue);
        }

        META_HPP_CHECK_INVOCABLE(LV(), m1, );
        META_HPP_CHECK_INVOCABLE(LV(), m2, &);
        META_HPP_CHECK_INVOCABLE(LV(), m3, &&);
        META_HPP_CHECK_INVOCABLE(LV(), m4, const);
        META_HPP_CHECK_INVOCABLE(LV(), m5, const &);
        META_HPP_CHECK_INVOCABLE(LV(), m6, const &&);

        META_HPP_CHECK_INVOCABLE(LV2(), m1, );
        META_HPP_CHECK_INVOCABLE(LV2(), m2, &);
        META_HPP_CHECK_INVOCABLE(LV2(), m3, &&);
        META_HPP_CHECK_INVOCABLE(LV2(), m4, const);
        META_HPP_CHECK_INVOCABLE(LV2(), m5, const &);
        META_HPP_CHECK_INVOCABLE(LV2(), m6, const &&);
    }

    {
        // const lvalue
        auto CLV = []() -> const clazz& { static clazz v; return v; };
        auto CLV2 = []() -> const dclazz& { static dclazz v; return v; };

        {
            meta::detail::inst i{CLV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == meta::detail::inst::ref_types::const_lvalue);
        }

        META_HPP_CHECK_INVOCABLE(CLV(), m1, );
        META_HPP_CHECK_INVOCABLE(CLV(), m2, &);
        META_HPP_CHECK_INVOCABLE(CLV(), m3, &&);
        META_HPP_CHECK_INVOCABLE(CLV(), m4, const);
        META_HPP_CHECK_INVOCABLE(CLV(), m5, const &);
        META_HPP_CHECK_INVOCABLE(CLV(), m6, const &&);

        META_HPP_CHECK_INVOCABLE(CLV2(), m1, );
        META_HPP_CHECK_INVOCABLE(CLV2(), m2, &);
        META_HPP_CHECK_INVOCABLE(CLV2(), m3, &&);
        META_HPP_CHECK_INVOCABLE(CLV2(), m4, const);
        META_HPP_CHECK_INVOCABLE(CLV2(), m5, const &);
        META_HPP_CHECK_INVOCABLE(CLV2(), m6, const &&);
    }

    {
        // xvalue
        auto XV = []() -> clazz&& { static clazz v; return std::move(v); };
        auto XV2 = []() -> dclazz&& { static dclazz v; return std::move(v); };

        {
            meta::detail::inst i{XV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == meta::detail::inst::ref_types::rvalue);
        }

        META_HPP_CHECK_INVOCABLE(XV(), m1, );
        META_HPP_CHECK_INVOCABLE(XV(), m2, &);
        META_HPP_CHECK_INVOCABLE(XV(), m3, &&);
        META_HPP_CHECK_INVOCABLE(XV(), m4, const);
        META_HPP_CHECK_INVOCABLE(XV(), m5, const &);
        META_HPP_CHECK_INVOCABLE(XV(), m6, const &&);

        META_HPP_CHECK_INVOCABLE(XV2(), m1, );
        META_HPP_CHECK_INVOCABLE(XV2(), m2, &);
        META_HPP_CHECK_INVOCABLE(XV2(), m3, &&);
        META_HPP_CHECK_INVOCABLE(XV2(), m4, const);
        META_HPP_CHECK_INVOCABLE(XV2(), m5, const &);
        META_HPP_CHECK_INVOCABLE(XV2(), m6, const &&);
    }

    {
        // const xvalue
        auto CXV = []() -> const clazz&& { static clazz v; return std::move(v); };
        auto CXV2 = []() -> const dclazz&& { static dclazz v; return std::move(v); };

        {
            meta::detail::inst i{CXV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == meta::detail::inst::ref_types::const_rvalue);
        }

        META_HPP_CHECK_INVOCABLE(CXV(), m1, );
        META_HPP_CHECK_INVOCABLE(CXV(), m2, &);
        META_HPP_CHECK_INVOCABLE(CXV(), m3, &&);
        META_HPP_CHECK_INVOCABLE(CXV(), m4, const);
        META_HPP_CHECK_INVOCABLE(CXV(), m5, const &);
        META_HPP_CHECK_INVOCABLE(CXV(), m6, const &&);

        META_HPP_CHECK_INVOCABLE(CXV2(), m1, );
        META_HPP_CHECK_INVOCABLE(CXV2(), m2, &);
        META_HPP_CHECK_INVOCABLE(CXV2(), m3, &&);
        META_HPP_CHECK_INVOCABLE(CXV2(), m4, const);
        META_HPP_CHECK_INVOCABLE(CXV2(), m5, const &);
        META_HPP_CHECK_INVOCABLE(CXV2(), m6, const &&);
    }

    {
        // prvalue
        auto PRV = []() -> clazz { return clazz{}; };
        auto PRV2 = []() -> dclazz { return dclazz{}; };

        {
            meta::detail::inst i{PRV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == meta::detail::inst::ref_types::rvalue);
        }

        META_HPP_CHECK_INVOCABLE(PRV(), m1, );
        META_HPP_CHECK_INVOCABLE(PRV(), m2, &);
        META_HPP_CHECK_INVOCABLE(PRV(), m3, &&);
        META_HPP_CHECK_INVOCABLE(PRV(), m4, const);
        META_HPP_CHECK_INVOCABLE(PRV(), m5, const &);
        META_HPP_CHECK_INVOCABLE(PRV(), m6, const &&);

        META_HPP_CHECK_INVOCABLE(PRV2(), m1, );
        META_HPP_CHECK_INVOCABLE(PRV2(), m2, &);
        META_HPP_CHECK_INVOCABLE(PRV2(), m3, &&);
        META_HPP_CHECK_INVOCABLE(PRV2(), m4, const);
        META_HPP_CHECK_INVOCABLE(PRV2(), m5, const &);
        META_HPP_CHECK_INVOCABLE(PRV2(), m6, const &&);
    }

    {
        // const prvalue
        auto CPRV = []() -> const clazz { return clazz{}; };
        auto CPRV2 = []() -> const dclazz { return dclazz{}; };

        {
            meta::detail::inst i{CPRV()};
            CHECK(i.get_raw_type() == meta::resolve_type<clazz>());
            CHECK(i.get_ref_type() == meta::detail::inst::ref_types::const_rvalue);
        }

        META_HPP_CHECK_INVOCABLE(CPRV(), m1, );
        META_HPP_CHECK_INVOCABLE(CPRV(), m2, &);
        META_HPP_CHECK_INVOCABLE(CPRV(), m3, &&);
        META_HPP_CHECK_INVOCABLE(CPRV(), m4, const);
        META_HPP_CHECK_INVOCABLE(CPRV(), m5, const &);
        META_HPP_CHECK_INVOCABLE(CPRV(), m6, const &&);

        META_HPP_CHECK_INVOCABLE(CPRV2(), m1, );
        META_HPP_CHECK_INVOCABLE(CPRV2(), m2, &);
        META_HPP_CHECK_INVOCABLE(CPRV2(), m3, &&);
        META_HPP_CHECK_INVOCABLE(CPRV2(), m4, const);
        META_HPP_CHECK_INVOCABLE(CPRV2(), m5, const &);
        META_HPP_CHECK_INVOCABLE(CPRV2(), m6, const &&);
    }
}
