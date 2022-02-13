/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/constructor_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_tag {};

    template < class_kind Class, typename... Args >
    // NOLINTNEXTLINE(readability-named-parameter)
    constructor_type_data::constructor_type_data(type_list<Class>, type_list<Args...>)
    : type_data_base{type_id{type_list<constructor_tag<Class, Args...>>{}}, type_kind::constructor_}
    , flags{constructor_traits<Class, Args...>::make_flags()}
    , class_type{resolve_type<typename constructor_traits<Class, Args...>::class_type>()}
    , argument_types{resolve_types(typename constructor_traits<Class, Args...>::argument_types{})} {}
}

namespace meta_hpp
{
    inline constructor_type::constructor_type(detail::constructor_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool constructor_type::is_valid() const noexcept {
        return !!data_;
    }

    inline constructor_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id constructor_type::get_id() const noexcept {
        return data_->id;
    }

    inline constructor_bitflags constructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& constructor_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t constructor_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type constructor_type::get_class_type() const noexcept {
        return data_->class_type;
    }

    inline any_type constructor_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const std::vector<any_type>& constructor_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}
