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
    inline ctor_index::ctor_index(ctor_type type)
    : type_{std::move(type)} {}

    template < detail::class_kind Class, typename... Args >
    ctor_index ctor_index::make() {
        return ctor_index{detail::resolve_ctor_type<Class, Args...>()};
    }

    inline const ctor_type& ctor_index::get_type() const noexcept {
        return type_;
    }

    inline bool operator<(const ctor_index& l, const ctor_index& r) noexcept {
        return l.type_ < r.type_;
    }

    inline bool operator==(const ctor_index& l, const ctor_index& r) noexcept {
        return l.type_ == r.type_;
    }
}
