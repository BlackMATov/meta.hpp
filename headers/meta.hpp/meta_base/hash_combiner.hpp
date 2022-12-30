/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    struct hash_combiner {
        template < typename T >
        [[nodiscard]] std::size_t operator()(const T& x) noexcept {
            return std::hash<T>{}(x);
        }

        template < typename T >
        [[nodiscard]] std::size_t operator()(std::size_t seed, const T& x) noexcept {
            // NOLINTNEXTLINE(*-magic-numbers)
            return (seed ^= std::hash<T>{}(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
        }
    };
}
