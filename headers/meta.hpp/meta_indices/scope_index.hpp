/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_states.hpp"

namespace meta_hpp
{
    inline scope_index::scope_index(std::string name)
    : name_{std::move(name)} {}

    inline std::string scope_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& scope_index::get_name() const& noexcept {
        return name_;
    }

    inline void scope_index::swap(scope_index& other) noexcept {
        std::swap(name_, other.name_);
    }

    inline std::size_t scope_index::get_hash() const noexcept {
        return detail::hash_composer{} << name_;
    }
}
