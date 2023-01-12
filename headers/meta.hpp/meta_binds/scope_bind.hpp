/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    inline scope_bind::scope_bind(std::string name, metadata_map metadata, local_tag)
    : state_{detail::scope_state::make(std::move(name), std::move(metadata))} {}

    inline scope_bind::scope_bind(std::string_view name, metadata_map metadata, static_tag)
    : state_{detail::state_access(resolve_scope(name))} {
        state_->metadata.swap(metadata);
        state_->metadata.merge(std::move(metadata));
    }

    inline scope_bind::operator scope() const noexcept {
        return scope{state_};
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
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        state_->functions.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    //
    // typedef_
    //

    template < typename Type >
    scope_bind& scope_bind::typedef_(std::string name) {
        state_->typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
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
