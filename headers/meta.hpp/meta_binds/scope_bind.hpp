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
    inline scope_bind::scope_bind(const scope& scope, metadata_map metadata)
    : state_bind_base{scope, std::move(metadata)} {}

    //
    // function_
    //

    template < detail::function_pointer_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(std::string name, Function function_ptr, Policy policy) {
        return function_(std::move(name), function_ptr, {}, policy);
    }

    template < detail::function_pointer_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(std::string name, Function function_ptr, function_opts opts, Policy) {
        auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, std::move(opts.metadata));

        META_HPP_ASSERT(                                     //
            opts.arguments.size() <= state->arguments.size() //
            && "provided arguments don't match function argument count"
        );

        for ( std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(get_state().functions, function{std::move(state)});
        return *this;
    }

    template < detail::function_pointer_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(std::string name, Function function_ptr, string_ilist arguments, Policy) {
        auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, {});

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match function argument count"
        );

        for ( std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        detail::insert_or_assign(get_state().functions, function{std::move(state)});
        return *this;
    }

    //
    // typedef_
    //

    template < typename Type >
    scope_bind& scope_bind::typedef_(std::string name) {
        get_state().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    //
    // variable_
    //

    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    scope_bind& scope_bind::variable_(std::string name, Pointer variable_ptr, Policy policy) {
        return variable_(std::move(name), variable_ptr, {}, policy);
    }

    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    scope_bind& scope_bind::variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy) {
        auto state = detail::variable_state::make<Policy>(std::move(name), variable_ptr, std::move(opts.metadata));
        detail::insert_or_assign(get_state().variables, variable{std::move(state)});
        return *this;
    }
}
