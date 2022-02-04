/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    concept state_family =
        stdex::same_as<T, ctor> ||
        stdex::same_as<T, dtor> ||
        stdex::same_as<T, evalue> ||
        stdex::same_as<T, function> ||
        stdex::same_as<T, member> ||
        stdex::same_as<T, method> ||
        stdex::same_as<T, scope> ||
        stdex::same_as<T, variable>;

    template < state_family T >
    [[nodiscard]] auto state_access(const T& state) {
        return state.state_;
    }
}
