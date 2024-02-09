/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_ucast.hpp"

#include "../meta_detail/base_info.hpp"
#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/value_utilities/utraits.hpp"

namespace meta_hpp
{
    template < typename To, typename From >
        requires detail::ucast_as_pointers<To, From>
    To ucast(From from) {
        using from_cv_data_type = std::remove_pointer_t<From>;
        using from_data_type = std::remove_cv_t<from_cv_data_type>;

        using to_cv_data_type = std::remove_pointer_t<To>;
        using to_data_type = std::remove_cv_t<to_cv_data_type>;

        static_assert(
            detail::poly_info_enabled<from_data_type>,
            "The type doesn't support ucasts. "
            "Use the META_HPP_ENABLE_POLY_INFO macro to fix it."
        );

        if ( from == nullptr ) {
            return nullptr;
        }

        if constexpr ( std::is_same_v<from_data_type, to_data_type> ) {
            return from;
        } else {
            detail::type_registry& registry{detail::type_registry::instance()};
            const detail::poly_info& meta_info{detail::get_meta_poly_info(registry, *from)};

            // NOLINTNEXTLINE(*-const-cast)
            void* most_derived_object_ptr = const_cast<void*>(meta_info.ptr);

            if constexpr ( std::is_void_v<to_data_type> ) {
                return most_derived_object_ptr;
            } else {
                const class_type& to_class_type = registry.resolve_by_type<to_data_type>();
                return static_cast<To>(detail::pointer_upcast(most_derived_object_ptr, meta_info.type, to_class_type));
            }
        }
    }

    template < typename To, typename From >
        requires detail::ucast_as_references<To, From>
    // NOLINTNEXTLINE(*-missing-std-forward)
    To ucast(From&& from) {
        using from_cv_data_type = std::remove_reference_t<From>;
        using to_cv_data_type = std::remove_reference_t<To>;

        static_assert(
            detail::poly_info_enabled<from_cv_data_type>,
            "The type doesn't support ucasts. "
            "Use the META_HPP_ENABLE_POLY_INFO macro to fix it."
        );

        if ( to_cv_data_type* ptr = ucast<to_cv_data_type*>(std::addressof(from)) ) {
            return *ptr;
        }

        throw_exception(error_code::bad_cast);
    }
}
