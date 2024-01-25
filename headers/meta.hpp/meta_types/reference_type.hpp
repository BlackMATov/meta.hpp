/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_traits/reference_traits.hpp"

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    reference_type_data::reference_type_data(type_list<Reference>)
    : type_data_base{type_kind::reference_}
    , flags{reference_traits<Reference>::make_flags()}
    , data_type{resolve_type<typename reference_traits<Reference>::data_type>()} {}
}

namespace meta_hpp
{
    inline reference_bitflags reference_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline any_type reference_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}
