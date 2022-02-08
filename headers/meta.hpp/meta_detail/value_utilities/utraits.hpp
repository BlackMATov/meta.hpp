/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_value.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    inline constexpr bool is_uvalue_kind_v =
        std::is_same_v<T, uarg_base> ||
        std::is_same_v<T, uarg> ||
        std::is_same_v<T, uinst_base> ||
        std::is_same_v<T, uinst> ||
        std::is_same_v<T, uvalue>;

    template < typename T >
    concept uvalue_kind = is_uvalue_kind_v<T>;

    template < typename T >
    concept decay_uvalue_kind = uvalue_kind<std::decay_t<T>>;

    template < typename T >
    concept decay_non_uvalue_kind = !decay_uvalue_kind<T>;
}

namespace meta_hpp::detail
{
    template < typename T >
    concept arg_lvalue_ref_kind =
        (decay_non_uvalue_kind<T>) &&
        (std::is_lvalue_reference_v<T>);

    template < typename T >
    concept arg_rvalue_ref_kind =
        (decay_non_uvalue_kind<T>) &&
        (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>);
}

namespace meta_hpp::detail
{
    template < typename T >
    concept inst_class_ref_kind =
        (std::is_class_v<T>) ||
        (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T >
    concept inst_class_lvalue_ref_kind =
        (decay_non_uvalue_kind<T>) &&
        (std::is_lvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T >
    concept inst_class_rvalue_ref_kind =
        (decay_non_uvalue_kind<T>) &&
        (std::is_class_v<T> || (std::is_rvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>));
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

        for ( auto&& [base, base_info] : type_access(from)->bases_info ) {
            if ( base == to ) {
                return base_info.upcast(ptr);
            }

            if ( base.is_derived_from(to) ) {
                return pointer_upcast(base_info.upcast(ptr), base, to);
            }
        }

        return nullptr;
    }

    template < class_kind To, class_kind From >
    [[nodiscard]] To* pointer_upcast(From* ptr) {
        return static_cast<To*>(pointer_upcast(ptr, resolve_type<From>(), resolve_type<To>()));
    }

    template < class_kind To, class_kind From >
    [[nodiscard]] const To* pointer_upcast(const From* ptr) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        return pointer_upcast<To>(const_cast<From*>(ptr));
    }
}
