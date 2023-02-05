/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_states.hpp"
#include "meta_types.hpp"

#include "meta_detail/state_registry.hpp"
#include "meta_detail/type_registry.hpp"

namespace meta_hpp
{
    template < typename F >
    void for_each_type(F&& f) {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        registry.for_each_type(std::forward<F>(f));
    }

    template < typename T >
    [[nodiscard]] auto resolve_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_type<std::remove_cv_t<T>>();
    }

    template < typename T >
    [[nodiscard]] auto resolve_type(T&&) {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_type<std::remove_cvref_t<T>>();
    }

    template < typename... Ts >
    [[nodiscard]] any_type_list resolve_types() {
        return {resolve_type<Ts>()...};
    }

    template < typename... Ts >
    [[nodiscard]] any_type_list resolve_types(type_list<Ts...>) {
        return {resolve_type<Ts>()...};
    }
}

namespace meta_hpp
{
    template < detail::class_kind Class, typename... Args >
    [[nodiscard]] constructor_type resolve_constructor_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_constructor_type<Class, Args...>();
    }

    template < detail::class_kind Class >
    [[nodiscard]] destructor_type resolve_destructor_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_destructor_type<Class>();
    }

    template < typename T >
    [[nodiscard]] any_type resolve_polymorphic_type(T&& v) noexcept {
#if !defined(META_HPP_NO_RTTI)
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.get_type_by_rtti(typeid(v));
#else
        (void)v;
        return any_type{};
#endif
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

    [[nodiscard]] inline scope resolve_scope(std::string_view name) {
        using namespace detail;
        state_registry& registry = state_registry::instance();
        return registry.resolve_scope(name);
    }
}
