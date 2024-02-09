/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_registry.hpp"

namespace meta_hpp::detail
{
    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_lvalue_ref_kind //
        = (!uvalue_family<Tp>)  //
        &&(std::is_lvalue_reference_v<T>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_rvalue_ref_kind //
        = (!uvalue_family<Tp>)  //
        &&(!std::is_reference_v<T> || std::is_rvalue_reference_v<T>);
}

namespace meta_hpp::detail
{
    template < typename T >
    concept inst_class_ref_kind //
        = (std::is_class_v<T>)  //
       || (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_lvalue_ref_kind    //
        = (!uvalue_family<Tp>)            //
        &&(std::is_lvalue_reference_v<T>) //
        &&(std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_rvalue_ref_kind                               //
        = (!uvalue_family<Tp>)                                       //
        &&(!std::is_reference_v<T> || std::is_rvalue_reference_v<T>) //
        &&(std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);
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

        if ( base_class && derived_class ) {
            const class_type_data& derived_data = *type_access(derived_class);
            const class_type_data::deep_upcasts_t& deep_upcasts = derived_data.deep_upcasts;

            for ( const auto& upcast : deep_upcasts ) {
                if ( upcast.target == base_class ) {
                    return upcast.upcast != nullptr;
                }
            }
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

        const class_type_data& from_data = *type_access(from);
        const class_type_data::deep_upcasts_t& deep_upcasts = from_data.deep_upcasts;

        for ( const auto& upcast : deep_upcasts ) {
            if ( upcast.target == to ) {
                return upcast.upcast != nullptr ? upcast.apply(ptr) : nullptr;
            }
        }

        return nullptr;
    }

    [[nodiscard]] inline const void* pointer_upcast(const void* ptr, const class_type& from, const class_type& to) {
        // NOLINTNEXTLINE(*-const-cast)
        return pointer_upcast(const_cast<void*>(ptr), from, to);
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

        if ( to_class && from_class ) {
            if ( void* base_ptr = pointer_upcast(ptr, from_class, to_class) ) {
                return base_ptr;
            }
        }

        return nullptr;
    }

    [[nodiscard]] inline const void* pointer_upcast(const void* ptr, const any_type& from, const any_type& to) {
        // NOLINTNEXTLINE(*-const-cast)
        return pointer_upcast(const_cast<void*>(ptr), from, to);
    }
}

namespace meta_hpp::detail
{
    template < typename To, typename From >
    [[nodiscard]] To* pointer_upcast(type_registry& registry, From* ptr) {
        return static_cast<To*>(pointer_upcast( //
            ptr,
            registry.resolve_by_type<From>(),
            registry.resolve_by_type<To>()
        ));
    }

    template < typename To, typename From >
    [[nodiscard]] const To* pointer_upcast(type_registry& registry, const From* ptr) {
        return static_cast<const To*>(pointer_upcast( //
            ptr,
            registry.resolve_by_type<From>(),
            registry.resolve_by_type<To>()
        ));
    }
}
