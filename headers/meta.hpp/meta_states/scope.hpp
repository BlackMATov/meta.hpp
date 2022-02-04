/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"
#include "../meta_types.hpp"

#include "../meta_types/class_type.hpp"
#include "../meta_types/enum_type.hpp"

#include "../meta_states/function.hpp"
#include "../meta_states/variable.hpp"

#include "../meta_detail/type_registry.hpp"

namespace meta_hpp::detail
{
    inline scope_state_ptr scope_state::make(std::string name) {
        return std::make_shared<scope_state>(scope_state{
            .index{scope_index::make(std::move(name))},
        });
    }
}

namespace meta_hpp
{
    inline scope::scope(detail::scope_state_ptr state)
    : state_{std::move(state)} {}

    inline bool scope::is_valid() const noexcept {
        return !!state_;
    }

    inline scope::operator bool() const noexcept {
        return is_valid();
    }

    inline const scope_index& scope::get_index() const noexcept {
        return state_->index;
    }

    inline const std::string& scope::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const class_map& scope::get_classes() const noexcept {
        return state_->classes;
    }

    inline const enum_map& scope::get_enums() const noexcept {
        return state_->enums;
    }

    inline const function_map& scope::get_functions() const noexcept {
        return state_->functions;
    }

    inline const variable_map& scope::get_variables() const noexcept {
        return state_->variables;
    }

    inline class_type scope::get_class(std::string_view name) const noexcept {
        if ( auto iter = state_->classes.find(name); iter != state_->classes.end() ) {
            return iter->second;
        }
        return class_type{};
    }

    inline enum_type scope::get_enum(std::string_view name) const noexcept {
        if ( auto iter = state_->enums.find(name); iter != state_->enums.end() ) {
            return iter->second;
        }
        return enum_type{};
    }

    inline function scope::get_function(std::string_view name) const noexcept {
        for ( auto&& [index, function] : state_->functions ) {
            if ( index.get_name() == name ) {
                return function;
            }
        }
        return function{};
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
        return get_function_with(name, {detail::resolve_type<Args>()...});
    }

    inline function scope::get_function_with(std::string_view name, const std::vector<any_type>& args) const noexcept {
        for ( auto&& [index, function] : state_->functions ) {
            if ( index.get_name() != name ) {
                continue;
            }

            if ( function.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& function_args = function.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), function_args.begin(), function_args.end()) ) {
                return function;
            }
        }
        return function{};
    }

    inline function scope::get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        for ( auto&& [index, function] : state_->functions ) {
            if ( index.get_name() != name ) {
                continue;
            }

            if ( function.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& function_args = function.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), function_args.begin(), function_args.end()) ) {
                return function;
            }
        }
        return function{};
    }
}
