/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_traits/pointer_traits.hpp"

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    pointer_type_data::pointer_type_data(type_list<Pointer>)
    : type_data_base{type_kind::pointer_}
    , flags{pointer_traits<Pointer>::make_flags()}
    , data_type{resolve_type<typename pointer_traits<Pointer>::data_type>()} {}
}

namespace meta_hpp
{
    inline pointer_bitflags pointer_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline any_type pointer_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}
