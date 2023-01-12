/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

namespace meta_hpp
{
    inline destructor_index::destructor_index(destructor_type type)
    : type_{type} {}

    template < detail::class_kind Class >
    destructor_index destructor_index::make() {
        return destructor_index{resolve_destructor_type<Class>()};
    }

    inline std::size_t destructor_index::get_hash() const noexcept {
        return detail::hash_combiner{}(type_);
    }

    inline const destructor_type& destructor_index::get_type() const noexcept {
        return type_;
    }

    inline bool operator<(const destructor_index& l, const destructor_index& r) noexcept {
        return l.type_ < r.type_;
    }

    inline bool operator==(const destructor_index& l, const destructor_index& r) noexcept {
        return l.type_ == r.type_;
    }

    inline bool operator!=(const destructor_index& l, const destructor_index& r) noexcept {
        return !(l == r);
    }
}
