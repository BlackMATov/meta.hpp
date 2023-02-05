/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_registry.hpp"
#include "../../meta_uvalue.hpp"

namespace meta_hpp::detail
{
    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_lvalue_ref_kind = //
        (!any_uvalue_kind<Tp>) &&
        (std::is_lvalue_reference_v<T>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_rvalue_ref_kind = //
        (!any_uvalue_kind<Tp>) &&
        (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>);
}

namespace meta_hpp::detail
{
    template < typename T >
    concept inst_class_ref_kind = //
        (std::is_class_v<T>) ||
        (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_lvalue_ref_kind = //
        (!any_uvalue_kind<Tp>) &&
        (std::is_lvalue_reference_v<T>) &&
        (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_rvalue_ref_kind = //
        (!any_uvalue_kind<Tp>) &&
        (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>) &&
        (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < inst_class_ref_kind Q, bool is_const, bool is_lvalue, bool is_rvalue >
        struct inst_traits_impl;

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, false, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)();
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() &;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() &&;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() const;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() const &;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() const &&;
        };
    }

    template < inst_class_ref_kind Q >
    struct inst_traits final : impl::inst_traits_impl<Q,
        cvref_traits<Q>::is_const,
        cvref_traits<Q>::is_lvalue,
        cvref_traits<Q>::is_rvalue> {};
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline void* pointer_upcast(void* ptr, const class_type& from, const class_type& to) {
        if ( nullptr == ptr || !from || !to ) {
            return nullptr;
        }

        if ( from == to ) {
            return ptr;
        }

        for ( auto&& [base_type, base_info] : type_access(from)->bases_info ) {
            if ( base_type == to ) {
                return base_info.upcast(ptr);
            }

            if ( base_type.is_derived_from(to) ) {
                return pointer_upcast(base_info.upcast(ptr), base_type, to);
            }
        }

        return nullptr;
    }

    [[nodiscard]] inline const void* pointer_upcast(const void* ptr, const class_type& from, const class_type& to) {
        // NOLINTNEXTLINE(*-const-cast)
        return pointer_upcast(const_cast<void*>(ptr), from, to);
    }

    template < class_kind To, class_kind From >
    [[nodiscard]] To* pointer_upcast(From* ptr) {
        return static_cast<To*>(pointer_upcast(ptr, resolve_type<From>(), resolve_type<To>()));
    }

    template < class_kind To, class_kind From >
    [[nodiscard]] const To* pointer_upcast(const From* ptr) {
        return static_cast<const To*>(pointer_upcast(ptr, resolve_type<From>(), resolve_type<To>()));
    }
}
