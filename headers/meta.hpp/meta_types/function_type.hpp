/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/function_traits.hpp"

namespace meta_hpp::detail
{
    template < function_kind Function >
    struct function_tag {};

    template < function_kind Function >
    // NOLINTNEXTLINE(readability-named-parameter)
    function_type_data::function_type_data(type_list<Function>)
    : type_data_base{type_id{type_list<function_tag<Function>>{}}, type_kind::function_}
    , flags{function_traits<Function>::make_flags()}
    , return_type{resolve_type<typename function_traits<Function>::return_type>()}
    , argument_types{resolve_types(typename function_traits<Function>::argument_types{})} {}
}

namespace meta_hpp
{
    inline function_type::function_type(detail::function_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool function_type::is_valid() const noexcept {
        return !!data_;
    }

    inline function_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id function_type::get_id() const noexcept {
        return data_->id;
    }

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

    inline const std::vector<any_type>& function_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}
