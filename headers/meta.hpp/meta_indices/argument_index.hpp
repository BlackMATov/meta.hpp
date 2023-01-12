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
    inline argument_index::argument_index(any_type type, std::size_t position)
    : type_{type}
    , position_{position} {}

    template < typename Argument >
    inline argument_index argument_index::make(std::size_t position) {
        return argument_index{resolve_type<Argument>(), position};
    }

    inline std::size_t argument_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), position_);
    }

    inline const any_type& argument_index::get_type() const noexcept {
        return type_;
    }

    inline std::size_t argument_index::get_position() const noexcept {
        return position_;
    }

    inline bool operator<(const argument_index& l, const argument_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && l.position_ < r.position_);
    }

    inline bool operator==(const argument_index& l, const argument_index& r) noexcept {
        return l.type_ == r.type_ && l.position_ == r.position_;
    }

    inline bool operator!=(const argument_index& l, const argument_index& r) noexcept {
        return !(l == r);
    }
}
