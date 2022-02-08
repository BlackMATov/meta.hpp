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
        std::is_same_v<T, argument> ||
        std::is_same_v<T, constructor> ||
        std::is_same_v<T, destructor> ||
        std::is_same_v<T, evalue> ||
        std::is_same_v<T, function> ||
        std::is_same_v<T, member> ||
        std::is_same_v<T, method> ||
        std::is_same_v<T, scope> ||
        std::is_same_v<T, variable>;

    template < typename T >
    concept state_family = is_state_family_v<T>;

    template < state_family T >
    [[nodiscard]] auto state_access(const T& state) {
        return state.state_;
    }
}
