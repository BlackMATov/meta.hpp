/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_states.hpp"
#include "../meta_types.hpp"

#include "../meta_types/class_type.hpp"
#include "../meta_types/enum_type.hpp"

#include "../meta_states/function.hpp"
#include "../meta_states/variable.hpp"

namespace meta_hpp::detail
{
    inline scope_state_ptr scope_state::make(std::string name, metadata_map metadata) {
        return std::make_shared<scope_state>(scope_state{
            .index{scope_index::make(std::move(name))},
            .metadata{std::move(metadata)},
        });
    }
}

namespace meta_hpp
{
    inline scope::scope(detail::scope_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline scope& scope::operator=(detail::scope_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool scope::is_valid() const noexcept {
        return !!state_;
    }

    inline scope::operator bool() const noexcept {
        return is_valid();
    }

    inline const scope_index& scope::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& scope::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const std::string& scope::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const function_map& scope::get_functions() const noexcept {
        return state_->functions;
    }

    inline const typedef_map& scope::get_typedefs() const noexcept {
        return state_->typedefs;
    }

    inline const variable_map& scope::get_variables() const noexcept {
        return state_->variables;
    }

    inline function scope::get_function(std::string_view name) const noexcept {
        for ( auto&& [index, function] : state_->functions ) {
            if ( index.get_name() == name ) {
                return function;
            }
        }
        return function{};
    }

    inline any_type scope::get_typedef(std::string_view name) const noexcept {
        for ( auto&& [index, type] : state_->typedefs ) {
            if ( index == name ) {
                return type;
            }
        }
        return any_type{};
    }

    inline variable scope::get_variable(std::string_view name) const noexcept {
        for ( auto&& [index, variable] : state_->variables ) {
            if ( index.get_name() == name ) {
                return variable;
            }
        }
        return variable{};
    }

    template < typename... Args >
    function scope::get_function_with(std::string_view name) const noexcept {
        return get_function_with(name, {resolve_type<Args>()...});
    }

    template < typename Iter >
    function scope::get_function_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( auto&& [index, function] : state_->functions ) {
            if ( index.get_name() != name ) {
                continue;
            }

            const any_type_list& args = function.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return function;
            }
        }
        return function{};
    }

    inline function scope::get_function_with(std::string_view name, std::span<const any_type> args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    inline function scope::get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }
}
