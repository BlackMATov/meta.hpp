/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_traits/destructor_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct destructor_tag {};

    template < class_kind Class >
    // NOLINTNEXTLINE(readability-named-parameter)
    destructor_type_data::destructor_type_data(type_list<Class>)
    : type_data_base{type_id{type_list<destructor_tag<Class>>{}}, type_kind::destructor_}
    , flags{destructor_traits<Class>::make_flags()}
    , class_type{resolve_type<typename destructor_traits<Class>::class_type>()} {}
}

namespace meta_hpp
{
    inline destructor_type::destructor_type(detail::destructor_type_data* data)
    : data_{data} {}

    inline bool destructor_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline destructor_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id destructor_type::get_id() const noexcept {
        return data_->id;
    }

    inline destructor_bitflags destructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& destructor_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline any_type destructor_type::get_class_type() const noexcept {
        return data_->class_type;
    }
}
