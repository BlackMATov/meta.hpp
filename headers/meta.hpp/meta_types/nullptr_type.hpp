/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
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

    template < nullptr_kind Nullptr >
    nullptr_type_data_ptr nullptr_type_data::get_static() {
        static nullptr_type_data_ptr data = std::make_shared<nullptr_type_data>(type_list<Nullptr>{});
        return data;
    }
}

namespace meta_hpp
{
    inline nullptr_type::nullptr_type(detail::nullptr_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool nullptr_type::is_valid() const noexcept {
        return !!data_;
    }

    inline nullptr_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id nullptr_type::get_id() const noexcept {
        return data_->id;
    }
}
