/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"

namespace meta_hpp
{
    inline constructor_index::constructor_index(constructor_type type)
    : type_{type} {}

    template < detail::class_kind Class, typename... Args >
    constructor_index constructor_index::make() {
        return constructor_index{detail::resolve_constructor_type<Class, Args...>()};
    }

    inline std::size_t constructor_index::get_hash() const noexcept {
        return detail::hash_combiner{}(type_);
    }

    inline const constructor_type& constructor_index::get_type() const noexcept {
        return type_;
    }

    inline bool operator<(const constructor_index& l, const constructor_index& r) noexcept {
        return l.type_ < r.type_;
    }

    inline bool operator==(const constructor_index& l, const constructor_index& r) noexcept {
        return l.type_ == r.type_;
    }
}
