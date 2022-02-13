/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_traits/number_traits.hpp"

namespace meta_hpp::detail
{
    template < number_kind Number >
    struct number_tag {};

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

    inline number_bitflags number_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& number_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t number_type::get_size() const noexcept {
        return data_->size;
    }
}
