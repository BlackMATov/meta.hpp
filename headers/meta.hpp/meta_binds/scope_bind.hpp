/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"

#include "../meta_detail/state_registry.hpp"
#include "../meta_detail/type_registry.hpp"

namespace meta_hpp
{
    // NOLINTNEXTLINE(readability-named-parameter)
    inline scope_bind::scope_bind(std::string name, local_tag)
    : state_{detail::scope_state::make(std::move(name))} {}

    // NOLINTNEXTLINE(readability-named-parameter)
    inline scope_bind::scope_bind(std::string_view name, static_tag)
    : state_{detail::state_access(detail::resolve_scope(name))} {}

    inline scope_bind::operator scope() const noexcept {
        return scope{state_};
    }

    template < detail::class_kind Class >
    scope_bind& scope_bind::class_(std::string name) {
        state_->classes.emplace(std::move(name), detail::resolve_type<Class>());
        return *this;
    }

    template < detail::enum_kind Enum >
    scope_bind& scope_bind::enum_(std::string name) {
        state_->enums.emplace(std::move(name), detail::resolve_type<Enum>());
        return *this;
    }

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(std::string name, Function function, Policy policy) {
        return function_(std::move(name), std::move(function), {}, policy);
    }

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(
        std::string name,
        Function function,
        std::initializer_list<std::string_view> pnames,
        [[maybe_unused]] Policy policy)
    {
        auto function_state = detail::function_state::make<Policy>(std::move(name), std::move(function));

        if ( pnames.size() > function_state->parameters.size() ) {
            detail::throw_exception_with("provided parameter names don't match function argument count");
        }

        for ( std::size_t i = 0; i < pnames.size(); ++i ) {
            parameter& param = function_state->parameters[i];
            detail::state_access(param)->name = std::string{std::data(pnames)[i]};
        }

        state_->functions.emplace(function_state->index, std::move(function_state));
        return *this;
    }

    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    scope_bind& scope_bind::variable_(std::string name, Pointer pointer, [[maybe_unused]] Policy policy) {
        auto variable_state = detail::variable_state::make<Policy>(std::move(name), std::move(pointer));
        state_->variables.emplace(variable_state->index, std::move(variable_state));
        return *this;
    }
}
