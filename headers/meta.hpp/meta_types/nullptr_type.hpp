/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

namespace meta_hpp::detail
{
    template < nullptr_kind Nullptr >
    struct nullptr_tag {};

    template < nullptr_kind Nullptr >
    // NOLINTNEXTLINE(readability-named-parameter)
    nullptr_type_data::nullptr_type_data(type_list<Nullptr>)
    : type_data_base{type_id{type_list<nullptr_tag<Nullptr>>{}}, type_kind::nullptr_} {}
}

namespace meta_hpp
{
    inline nullptr_type::nullptr_type(detail::nullptr_type_data* data)
    : data_{data} {}

    inline bool nullptr_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline nullptr_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id nullptr_type::get_id() const noexcept {
        return data_->id;
    }

    inline const metadata_map& nullptr_type::get_metadata() const noexcept {
        return data_->metadata;
    }
}
