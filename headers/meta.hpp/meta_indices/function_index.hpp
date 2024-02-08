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
    inline function_index::function_index(function_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline function_type function_index::get_type() const noexcept {
        return type_;
    }

    inline std::string function_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& function_index::get_name() const& noexcept {
        return name_;
    }

    inline void function_index::swap(function_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t function_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << name_;
    }
}
