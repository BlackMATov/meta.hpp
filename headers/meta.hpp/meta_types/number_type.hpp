/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_traits/number_traits.hpp"

namespace meta_hpp::detail
{
    template < number_kind Number >
    struct number_tag {};

    template < number_kind Number >
    number_type_data_ptr number_type_data::get_static() {
        static number_type_data_ptr data = std::make_shared<number_type_data>(type_list<Number>{});
        return data;
    }

    template < number_kind Number >
    // NOLINTNEXTLINE(readability-named-parameter)
    number_type_data::number_type_data(type_list<Number>)
    : type_data_base{type_id{type_list<number_tag<Number>>{}}, type_kind::number_}
    , flags{number_traits<Number>::make_flags()}
    , size{number_traits<Number>::size} {}
}

namespace meta_hpp
{
    inline number_type::number_type(detail::number_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool number_type::is_valid() const noexcept {
        return !!data_;
    }

    inline number_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id number_type::get_id() const noexcept {
        return data_->id;
    }

    inline bitflags<number_flags> number_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t number_type::get_size() const noexcept {
        return data_->size;
    }
}
