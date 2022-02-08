/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/member_traits.hpp"

namespace meta_hpp::detail
{
    template < member_kind Member >
    struct member_tag {};

    template < member_kind Member >
    // NOLINTNEXTLINE(readability-named-parameter)
    member_type_data::member_type_data(type_list<Member>)
    : type_data_base{type_id{type_list<member_tag<Member>>{}}, type_kind::member_}
    , flags{member_traits<Member>::make_flags()}
    , owner_type{resolve_type<typename member_traits<Member>::class_type>()}
    , value_type{resolve_type<typename member_traits<Member>::value_type>()} {}
}

namespace meta_hpp
{
    inline member_type::member_type(detail::member_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool member_type::is_valid() const noexcept {
        return !!data_;
    }

    inline member_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id member_type::get_id() const noexcept {
        return data_->id;
    }

    inline member_bitflags member_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline class_type member_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type member_type::get_value_type() const noexcept {
        return data_->value_type;
    }
}
