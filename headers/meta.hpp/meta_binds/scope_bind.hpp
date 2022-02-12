/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"

#include "../meta_detail/state_registry.hpp"
#include "../meta_detail/type_registry.hpp"

namespace meta_hpp
{
    // NOLINTNEXTLINE(readability-named-parameter)
    inline scope_bind::scope_bind(std::string name, scope_opts opts, local_tag)
    : state_{detail::scope_state::make(std::move(name), std::move(opts.metadata))} {}

    // NOLINTNEXTLINE(readability-named-parameter)
    inline scope_bind::scope_bind(std::string_view name, scope_opts opts, static_tag)
    : state_{detail::state_access(detail::resolve_scope(name))} {
        state_->metadata.swap(opts.metadata);
        state_->metadata.merge(opts.metadata);
    }

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

    //
    // function_
    //

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(
        std::string name,
        Function function,
        Policy policy)
    {
        return function_(std::move(name), std::move(function), {}, policy);
    }

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(
        std::string name,
        Function function,
        function_opts opts,
        [[maybe_unused]] Policy policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided arguments don't match function argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        state_->functions.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(
        std::string name,
        Function function,
        std::initializer_list<std::string_view> arguments,
        [[maybe_unused]] Policy policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            {});

        if ( arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match function argument count");
        }

        for ( std::size_t i = 0; i < arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        state_->functions.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    //
    // variable_
    //

    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    scope_bind& scope_bind::variable_(
        std::string name,
        Pointer pointer,
        Policy policy)
    {
        return variable_(std::move(name), std::move(pointer), {}, policy);
    }

    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    scope_bind& scope_bind::variable_(
        std::string name,
        Pointer pointer,
        variable_opts opts,
        [[maybe_unused]] Policy policy)
    {
        auto state = detail::variable_state::make<Policy>(
            std::move(name),
            std::move(pointer),
            std::move(opts.metadata));
        state_->variables.insert_or_assign(state->index, std::move(state));
        return *this;
    }
}
