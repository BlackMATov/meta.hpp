/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    // REFERENCE:
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

    template < std::size_t SizeBytes = sizeof(std::size_t) >
    struct fnv1a_hash_traits;

    template <>
    struct fnv1a_hash_traits<sizeof(std::uint32_t)> {
        using underlying_type = std::uint32_t;
        static inline constexpr underlying_type prime{16777619U};
        static inline constexpr underlying_type offset_basis{2166136261U};
    };

    template <>
    struct fnv1a_hash_traits<sizeof(std::uint64_t)> {
        using underlying_type = std::uint64_t;
        static inline constexpr underlying_type prime{1099511628211U};
        static inline constexpr underlying_type offset_basis{14695981039346656037U};
    };

    template < typename T >
        requires(std::is_same_v<T, std::byte>) || (std::is_integral_v<T> && sizeof(T) == 1)
    constexpr std::size_t fnv1a_hash(const T* mem, std::size_t size) noexcept {
        using traits = fnv1a_hash_traits<>;
        std::size_t hash{traits::offset_basis};
        for ( T byte : std::span(mem, size) ) {
            hash ^= static_cast<std::size_t>(byte);
            hash *= traits::prime;
        }
        return hash;
    }

    inline std::size_t fnv1a_hash(const void* mem, std::size_t size) noexcept {
        return fnv1a_hash(static_cast<const std::byte*>(mem), size);
    }
}
