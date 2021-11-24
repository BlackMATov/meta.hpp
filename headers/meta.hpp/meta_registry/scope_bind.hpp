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
    inline scope_bind::scope_bind(std::string_view name)
    : state_{detail::get_scope_state(name)} {}

    inline scope_bind::operator scope() const noexcept {
        return scope{state_};
    }

    template < class_kind Class >
    scope_bind& scope_bind::class_(std::string name) {
        auto class_data = detail::class_type_data::get<Class>();
        state_->classes.emplace(std::move(name), class_data);
        return *this;
    }

    template < enum_kind Enum >
    scope_bind& scope_bind::enum_(std::string name) {
        auto enum_data = detail::enum_type_data::get<Enum>();
        state_->enums.emplace(std::move(name), enum_data);
        return *this;
    }

    template < function_kind Function >
    scope_bind& scope_bind::function_(std::string name, Function function) {
        auto function_state = detail::function_state::make<Function>(std::move(name), std::move(function));
        state_->functions.emplace(function_state->index, std::move(function_state));
        return *this;
    }

    template < pointer_kind Pointer >
    scope_bind& scope_bind::variable_(std::string name, Pointer pointer) {
        auto variable_state = detail::variable_state::make<Pointer>(std::move(name), std::move(pointer));
        state_->variables.emplace(variable_state->index, std::move(variable_state));
        return *this;
    }
}
