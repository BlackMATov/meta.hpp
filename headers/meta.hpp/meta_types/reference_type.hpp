/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_traits/reference_traits.hpp"

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    struct reference_tag {};

    template < reference_kind Reference >
    reference_type_data::reference_type_data(type_list<Reference>)
    : type_data_base{type_id{type_list<reference_tag<Reference>>{}}, type_kind::reference_}
    , flags{reference_traits<Reference>::make_flags()}
    , data_type{resolve_type<typename reference_traits<Reference>::data_type>()} {}
}

namespace meta_hpp
{
    inline reference_type::reference_type(data_ptr data)
    : data_{data} {}

    inline bool reference_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline reference_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id reference_type::get_id() const noexcept {
        return data_->id;
    }

    inline reference_bitflags reference_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& reference_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline any_type reference_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}
