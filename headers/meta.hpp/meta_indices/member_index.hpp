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
    inline member_index::member_index(member_type type, std::string name)
    : type_{std::move(type)}
    , name_{std::move(name)} {}

    template < detail::member_kind Member >
    member_index member_index::make(std::string name) {
        return member_index{detail::resolve_type<Member>(), std::move(name)};
    }

    inline const member_type& member_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& member_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const member_index& l, const member_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && l.name_ < r.name_);
    }

    inline bool operator==(const member_index& l, const member_index& r) noexcept {
        return l.type_ == r.type_ && l.name_ == r.name_;
    }
}
