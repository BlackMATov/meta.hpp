/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_sharing.hpp"
#include "../meta_detail/type_traits/function_traits.hpp"

namespace meta_hpp::detail::function_type_data_impl
{
    template < function_kind Function >
    any_type_list make_argument_types() {
        using ft = function_traits<Function>;
        using ft_argument_types = typename ft::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<ft_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, ft_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<ft_argument_types>>());
    }
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    function_type_data::function_type_data(function_traits<Function>)
    : type_data_base{type_kind::function_, shared_traits_hash<function_traits<Function>>{}(this)}
    , flags{function_traits<Function>::make_flags()}
    , return_type{resolve_type<typename function_traits<Function>::return_type>()}
    , argument_types(function_type_data_impl::make_argument_types<Function>()) {}
}

namespace meta_hpp
{
    inline function_bitflags function_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t function_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type function_type::get_return_type() const noexcept {
        return data_->return_type;
    }

    inline any_type function_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& function_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}
