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
    inline method_index::method_index(method_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    template < detail::method_kind Method >
    method_index method_index::make(std::string name) {
        return method_index{detail::resolve_type<Method>(), std::move(name)};
    }

    inline std::size_t method_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }

    inline const method_type& method_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& method_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const method_index& l, const method_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const method_index& l, const method_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }
}
