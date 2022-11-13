/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    inline constexpr bool is_state_family_v =
        stdex::same_as<T, argument> ||
        stdex::same_as<T, constructor> ||
        stdex::same_as<T, destructor> ||
        stdex::same_as<T, evalue> ||
        stdex::same_as<T, function> ||
        stdex::same_as<T, member> ||
        stdex::same_as<T, method> ||
        stdex::same_as<T, scope> ||
        stdex::same_as<T, variable>;

    template < typename T >
    concept state_family = is_state_family_v<T>;

    template < state_family T >
    [[nodiscard]] auto state_access(const T& state) {
        return state.state_;
    }
}
