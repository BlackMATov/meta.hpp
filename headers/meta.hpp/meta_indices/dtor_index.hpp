/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"

namespace meta_hpp
{
    inline dtor_index::dtor_index(dtor_type type)
    : type_{std::move(type)} {}

    template < detail::class_kind Class >
    dtor_index dtor_index::make() {
        return dtor_index{detail::resolve_dtor_type<Class>()};
    }

    inline const dtor_type& dtor_index::get_type() const noexcept {
        return type_;
    }

    inline bool operator<(const dtor_index& l, const dtor_index& r) noexcept {
        return l.type_ < r.type_;
    }

    inline bool operator==(const dtor_index& l, const dtor_index& r) noexcept {
        return l.type_ == r.type_;
    }
}
