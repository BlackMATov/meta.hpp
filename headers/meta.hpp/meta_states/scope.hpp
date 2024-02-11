/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_states/function.hpp"
#include "../meta_states/variable.hpp"
#include "../meta_types/class_type.hpp"
#include "../meta_types/enum_type.hpp"

namespace meta_hpp::detail
{
    inline scope_state::scope_state(scope_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    inline scope_state::state_ptr scope_state::make(std::string name, metadata_map metadata) {
        scope_state state{
            scope_index{std::move(name)},
            std::move(metadata),
        };
        return std::make_shared<scope_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline const std::string& scope::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const function_list& scope::get_functions() const noexcept {
        return state_->functions;
    }

    inline const typedef_map& scope::get_typedefs() const noexcept {
        return state_->typedefs;
    }

    inline const variable_list& scope::get_variables() const noexcept {
        return state_->variables;
    }

    inline function scope::get_function(std::string_view name) const noexcept {
        for ( const function& function : state_->functions ) {
            if ( function.get_name() == name ) {
                return function;
            }
        }
        return function{};
    }

    inline any_type scope::get_typedef(std::string_view name) const noexcept {
        if ( auto iter{state_->typedefs.find(name)}; iter != state_->typedefs.end() ) {
            return iter->second;
        }
        return any_type{};
    }

    inline variable scope::get_variable(std::string_view name) const noexcept {
        for ( const variable& variable : state_->variables ) {
            if ( variable.get_name() == name ) {
                return variable;
            }
        }
        return variable{};
    }

    template < typename... Args >
    function scope::get_function_with( //
        std::string_view name
    ) const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_function_with(name, {registry.resolve_by_type<Args>()...});
    }

    template < typename Iter >
    function scope::get_function_with( //
        std::string_view name,
        Iter first,
        Iter last
    ) const {
        for ( const function& function : state_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const function_type& function_type = function.get_type();
            const any_type_list& function_args = function_type.get_argument_types();

            if ( std::equal(first, last, function_args.begin(), function_args.end()) ) {
                return function;
            }
        }
        return function{};
    }

    inline function scope::get_function_with( //
        std::string_view name,
        std::span<const any_type> args
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    inline function scope::get_function_with( //
        std::string_view name,
        std::initializer_list<any_type> args
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }
}
