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
    inline parameter_index::parameter_index(any_type type, std::string name)
    : type_{std::move(type)}
    , name_{std::move(name)} {}

    template < typename Parameter >
    inline parameter_index parameter_index::make(std::string name) {
        return parameter_index{detail::resolve_type<Parameter>(), std::move(name)};
    }

    inline const any_type& parameter_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& parameter_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const parameter_index& l, const parameter_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const parameter_index& l, const parameter_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }
}
