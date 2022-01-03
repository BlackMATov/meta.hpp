/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_traits/method_traits.hpp"

namespace meta_hpp::detail
{
    template < method_kind Method >
    struct method_tag {};

    template < method_kind Method >
    method_type_data_ptr method_type_data::get_static() {
        static method_type_data_ptr data = std::make_shared<method_type_data>(type_list<Method>{});
        return data;
    }

    template < method_kind Method >
    // NOLINTNEXTLINE(readability-named-parameter)
    method_type_data::method_type_data(type_list<Method>)
    : type_data_base{type_id{type_list<method_tag<Method>>{}}, type_kind::method_}
    , flags{method_traits<Method>::make_flags()}
    , owner_type{resolve_type<typename method_traits<Method>::class_type>()}
    , return_type{resolve_type<typename method_traits<Method>::return_type>()}
    , argument_types{method_traits<Method>::make_argument_types()} {}
}

namespace meta_hpp
{
    inline method_type::method_type(detail::method_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool method_type::is_valid() const noexcept {
        return !!data_;
    }

    inline method_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id method_type::get_id() const noexcept {
        return data_->id;
    }

    inline bitflags<method_flags> method_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t method_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline class_type method_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type method_type::get_return_type() const noexcept {
        return data_->return_type;
    }

    inline any_type method_type::get_argument_type(std::size_t index) const noexcept {
        return index < data_->argument_types.size() ? data_->argument_types[index] : any_type{};
    }

    inline const std::vector<any_type>& method_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}
