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
    inline evalue_index::evalue_index(enum_type type, std::string name)
    : type_{std::move(type)}
    , name_{std::move(name)} {}

    template < detail::enum_kind Enum >
    evalue_index evalue_index::make(std::string name) {
        return evalue_index{detail::resolve_type<Enum>(), std::move(name)};
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
