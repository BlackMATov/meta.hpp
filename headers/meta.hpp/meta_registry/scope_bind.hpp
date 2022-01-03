/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    // NOLINTNEXTLINE(readability-named-parameter)
    inline scope_bind::scope_bind(std::string name, local_tag)
    : state_{detail::scope_state::make(std::move(name))} {}

    // NOLINTNEXTLINE(readability-named-parameter)
    inline scope_bind::scope_bind(std::string_view name, static_tag)
    : state_{detail::scope_state::get_static(name)} {}

    inline scope_bind::operator scope() const noexcept {
        return scope{state_};
    }

    template < detail::class_kind Class >
    scope_bind& scope_bind::class_(std::string name) {
        state_->classes.emplace(std::move(name), resolve_type<Class>());
        return *this;
    }

    template < detail::enum_kind Enum >
    scope_bind& scope_bind::enum_(std::string name) {
        state_->enums.emplace(std::move(name), resolve_type<Enum>());
        return *this;
    }

    template < detail::function_kind Function >
    scope_bind& scope_bind::function_(std::string name, Function function) {
        auto function_state = detail::function_state::make<Function>(std::move(name), std::move(function));
        state_->functions.emplace(function_state->index, std::move(function_state));
        return *this;
    }

    template < detail::pointer_kind Pointer >
    scope_bind& scope_bind::variable_(std::string name, Pointer pointer) {
        auto variable_state = detail::variable_state::make<Pointer>(std::move(name), std::move(pointer));
        state_->variables.emplace(variable_state->index, std::move(variable_state));
        return *this;
    }
}
