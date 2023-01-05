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
    inline evalue_index::evalue_index(enum_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    template < detail::enum_kind Enum >
    evalue_index evalue_index::make(std::string name) {
        return evalue_index{resolve_type<Enum>(), std::move(name)};
    }

    inline std::size_t evalue_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }

    inline const enum_type& evalue_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& evalue_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const evalue_index& l, const evalue_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const evalue_index& l, const evalue_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }
}
