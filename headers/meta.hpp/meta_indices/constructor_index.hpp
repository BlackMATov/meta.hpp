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
    inline constructor_index::constructor_index(constructor_type type)
    : type_{type} {}

    inline const constructor_type& constructor_index::get_type() const noexcept {
        return type_;
    }

    inline std::size_t constructor_index::get_hash() const noexcept {
        return detail::hash_combiner{}(type_);
    }
}
