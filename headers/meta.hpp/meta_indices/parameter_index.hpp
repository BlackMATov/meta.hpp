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
    inline parameter_index::parameter_index(any_type type, std::size_t position)
    : type_{std::move(type)}
    , position_{position} {}

    template < typename Parameter >
    inline parameter_index parameter_index::make(std::size_t position) {
        return parameter_index{detail::resolve_type<Parameter>(), position};
    }

    inline const any_type& parameter_index::get_type() const noexcept {
        return type_;
    }

    inline std::size_t parameter_index::get_position() const noexcept {
        return position_;
    }

    inline bool operator<(const parameter_index& l, const parameter_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && l.position_ < r.position_);
    }

    inline bool operator==(const parameter_index& l, const parameter_index& r) noexcept {
        return l.type_ == r.type_ && l.position_ == r.position_;
    }
}
