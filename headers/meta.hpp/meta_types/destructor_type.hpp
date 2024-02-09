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
#include "../meta_detail/type_traits/destructor_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_type_data::destructor_type_data(destructor_traits<Class>)
    : type_data_base{type_kind::destructor_, shared_traits_hash<destructor_traits<Class>>{}(this)}
    , flags{destructor_traits<Class>::make_flags()}
    , owner_type{resolve_type<typename destructor_traits<Class>::class_type>()} {}
}

namespace meta_hpp
{
    inline destructor_bitflags destructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline class_type destructor_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }
}
