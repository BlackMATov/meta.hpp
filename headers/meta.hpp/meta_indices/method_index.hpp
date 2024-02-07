/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_types.hpp"

namespace meta_hpp
{
    inline method_index::method_index(method_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline method_type method_index::get_type() const noexcept {
        return type_;
    }

    inline std::string method_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& method_index::get_name() const& noexcept {
        return name_;
    }

    inline void method_index::swap(method_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t method_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << name_;
    }
}
