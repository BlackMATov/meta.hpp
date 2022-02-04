/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/dtor_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct dtor_tag {};

    template < class_kind Class >
    // NOLINTNEXTLINE(readability-named-parameter)
    dtor_type_data::dtor_type_data(type_list<Class>)
    : type_data_base{type_id{type_list<dtor_tag<Class>>{}}, type_kind::dtor_}
    , flags{dtor_traits<Class>::make_flags()}
    , class_type{resolve_type<typename dtor_traits<Class>::class_type>()} {}
}

namespace meta_hpp
{
    inline dtor_type::dtor_type(detail::dtor_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool dtor_type::is_valid() const noexcept {
        return !!data_;
    }

    inline dtor_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id dtor_type::get_id() const noexcept {
        return data_->id;
    }

    inline bitflags<dtor_flags> dtor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline any_type dtor_type::get_class_type() const noexcept {
        return data_->class_type;
    }
}
