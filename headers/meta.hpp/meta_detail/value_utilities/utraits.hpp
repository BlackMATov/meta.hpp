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
    template < typename T >
    concept any_uvalue_kind             //
        = std::is_same_v<T, uarg_base>  //
       || std::is_same_v<T, uarg>       //
       || std::is_same_v<T, uinst_base> //
       || std::is_same_v<T, uinst>      //
       || std::is_same_v<T, uresult>    //
       || std::is_same_v<T, uvalue>;    //
}

namespace meta_hpp::detail
{
    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_lvalue_ref_kind  //
        = (!any_uvalue_kind<Tp>) //
       && (std::is_lvalue_reference_v<T>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_rvalue_ref_kind  //
        = (!any_uvalue_kind<Tp>) //
       && (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>);
}

namespace meta_hpp::detail
{
    template < typename T >
    concept inst_class_ref_kind //
        = (std::is_class_v<T>)  //
       || (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_lvalue_ref_kind    //
        = (!any_uvalue_kind<Tp>)          //
       && (std::is_lvalue_reference_v<T>) //
       && (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_rvalue_ref_kind                               //
        = (!any_uvalue_kind<Tp>)                                     //
       && (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>) //
       && (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);
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
            using method_type = void (class_type::*)();
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() &;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() &&;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() const;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() const&;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() const&&;
        };
    }

    template < inst_class_ref_kind Q >
    struct inst_traits final
    : impl::inst_traits_impl< //
          Q,
          cvref_traits<Q>::is_const,
          cvref_traits<Q>::is_lvalue,
          cvref_traits<Q>::is_rvalue> {};
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline bool is_a(const any_type& base, const any_type& derived) noexcept {
        if ( base == derived ) {
            return true;
        }

        const class_type& base_class = base.as_class();
        const class_type& derived_class = derived.as_class();

        if ( base_class && derived_class && base_class.is_base_of(derived_class) ) {
            return true;
        }

        return false;
    }
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
    [[nodiscard]] To* pointer_upcast(type_registry& registry, From* ptr) {
        const class_type& to_class = registry.resolve_type<To>();
        const class_type& from_class = registry.resolve_type<From>();
        return static_cast<To*>(pointer_upcast(ptr, from_class, to_class));
    }

    template < class_kind To, class_kind From >
    [[nodiscard]] const To* pointer_upcast(type_registry& registry, const From* ptr) {
        const class_type& to_class = registry.resolve_type<To>();
        const class_type& from_class = registry.resolve_type<From>();
        return static_cast<const To*>(pointer_upcast(ptr, from_class, to_class));
    }
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline void* pointer_upcast(void* ptr, const any_type& from, const any_type& to) {
        if ( nullptr == ptr || !from || !to ) {
            return nullptr;
        }

        if ( to.is_void() || from == to ) {
            return ptr;
        }

        const class_type& to_class = to.as_class();
        const class_type& from_class = from.as_class();

        if ( to_class && from_class && from_class.is_derived_from(to_class) ) {
            return pointer_upcast(ptr, from_class, to_class);
        }

        return nullptr;
    }

    [[nodiscard]] inline const void* pointer_upcast(const void* ptr, const any_type& from, const any_type& to) {
        // NOLINTNEXTLINE(*-const-cast)
        return pointer_upcast(const_cast<void*>(ptr), from, to);
    }
}
