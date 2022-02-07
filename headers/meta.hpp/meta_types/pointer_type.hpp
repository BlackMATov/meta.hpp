/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/pointer_traits.hpp"

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    struct pointer_tag {};

    template < pointer_kind Pointer >
    // NOLINTNEXTLINE(readability-named-parameter)
    pointer_type_data::pointer_type_data(type_list<Pointer>)
    : type_data_base{type_id{type_list<pointer_tag<Pointer>>{}}, type_kind::pointer_}
    , flags{pointer_traits<Pointer>::make_flags()}
    , data_type{resolve_type<typename pointer_traits<Pointer>::data_type>()} {}
}

namespace meta_hpp
{
    inline pointer_type::pointer_type(detail::pointer_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool pointer_type::is_valid() const noexcept {
        return !!data_;
    }

    inline pointer_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id pointer_type::get_id() const noexcept {
        return data_->id;
    }

    inline pointer_bitflags pointer_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline any_type pointer_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}
