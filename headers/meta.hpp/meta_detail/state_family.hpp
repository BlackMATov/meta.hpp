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
        std::same_as<T, argument> ||
        std::same_as<T, constructor> ||
        std::same_as<T, destructor> ||
        std::same_as<T, evalue> ||
        std::same_as<T, function> ||
        std::same_as<T, member> ||
        std::same_as<T, method> ||
        std::same_as<T, scope> ||
        std::same_as<T, variable>;

    template < typename T >
    concept state_family = is_state_family_v<T>;

    template < state_family T >
    [[nodiscard]] auto state_access(const T& state) {
        return state.state_;
    }
}
