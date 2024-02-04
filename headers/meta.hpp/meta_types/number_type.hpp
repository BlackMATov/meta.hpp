/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_sharing.hpp"
#include "../meta_detail/type_traits/number_traits.hpp"

namespace meta_hpp::detail
{
    template < number_kind Number >
    number_type_data::number_type_data(type_list<Number>)
    : type_data_base{type_kind::number_, shared_type_data_hash<type_kind::number_, Number>{}(this)}
    , flags{number_traits<Number>::make_flags()}
    , size{number_traits<Number>::size}
    , align{number_traits<Number>::align} {}
}

namespace meta_hpp
{
    inline number_bitflags number_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t number_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t number_type::get_align() const noexcept {
        return data_->align;
    }
}
