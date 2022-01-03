/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_traits/ctor_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct ctor_tag {};

    template < class_kind Class, typename... Args >
    // NOLINTNEXTLINE(readability-named-parameter)
    ctor_type_data::ctor_type_data(type_list<Class>, type_list<Args...>)
    : type_data_base{type_id{type_list<ctor_tag<Class, Args...>>{}}, type_kind::ctor_}
    , flags{ctor_traits<Class, Args...>::make_flags()}
    , class_type{resolve_type<typename ctor_traits<Class, Args...>::class_type>()}
    , argument_types{ctor_traits<Class, Args...>::make_argument_types()} {}

    template < class_kind Class, typename... Args >
    ctor_type_data_ptr ctor_type_data::get_static() {
        static ctor_type_data_ptr data = std::make_shared<ctor_type_data>(type_list<Class>{}, type_list<Args...>{});
        return data;
    }
}

namespace meta_hpp
{
    inline ctor_type::ctor_type(detail::ctor_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool ctor_type::is_valid() const noexcept {
        return !!data_;
    }

    inline ctor_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id ctor_type::get_id() const noexcept {
        return data_->id;
    }

    inline bitflags<ctor_flags> ctor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t ctor_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type ctor_type::get_class_type() const noexcept {
        return data_->class_type;
    }

    inline any_type ctor_type::get_argument_type(std::size_t index) const noexcept {
        return index < data_->argument_types.size() ? data_->argument_types[index] : any_type{};
    }

    inline const std::vector<any_type>& ctor_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}
