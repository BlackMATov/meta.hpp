/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_details.hpp"
#include "meta_states.hpp"
#include "meta_types.hpp"

#include "meta_detail/base_info.hpp"
#include "meta_detail/state_registry.hpp"
#include "meta_detail/type_registry.hpp"

namespace meta_hpp
{
    template < type_family Type = any_type, typename F >
    // NOLINTNEXTLINE(*-missing-std-forward)
    void for_each_type(F&& f) {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        registry.for_each_type([&f](const any_type& type) {
            if ( type.is<Type>() ) {
                std::invoke(f, type.as<Type>());
            }
        });
    }

    template < typename T >
    auto resolve_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_by_type<std::remove_cv_t<T>>();
    }

    template < typename T >
    // NOLINTNEXTLINE(*-missing-std-forward)
    auto resolve_type(T&& from) {
        using namespace detail;

        using raw_type = std::remove_cvref_t<T>;
        type_registry& registry = type_registry::instance();

        if constexpr ( std::is_class_v<raw_type> && std::is_polymorphic_v<raw_type> ) {
            static_assert(
                detail::poly_info_enabled<T>,
                "The class doesn't support polymorphic type resolving. "
                "Use the META_HPP_ENABLE_POLY_INFO macro to fix it."
            );
            return detail::get_meta_poly_info(registry, from).type;
        } else {
            (void)from;
            return registry.resolve_by_type<raw_type>();
        }
    }
}

namespace meta_hpp
{
    template < typename F >
    void for_each_scope(F&& f) {
        using namespace detail;
        state_registry& registry = state_registry::instance();
        registry.for_each_scope(std::forward<F>(f));
    }

    inline scope resolve_scope(std::string_view name) {
        using namespace detail;
        state_registry& registry = state_registry::instance();
        return registry.resolve_scope(name);
    }
}
