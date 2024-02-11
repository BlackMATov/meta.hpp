/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"
#include "bitflags.hpp"
#include "fnv1a_hash.hpp"

namespace meta_hpp::detail
{
    template < std::size_t SizeBytes = sizeof(std::size_t) >
    struct hash_composer_traits {};

    template <>
    struct hash_composer_traits<sizeof(std::uint32_t)> {
        [[nodiscard]] static constexpr std::size_t combine(std::size_t a, std::size_t b) noexcept {
            // NOLINTNEXTLINE(*-magic-numbers)
            return (a ^= b + 0x9e3779b9U + (a << 6) + (a >> 2));
        }
    };

    template <>
    struct hash_composer_traits<sizeof(std::uint64_t)> {
        [[nodiscard]] static constexpr std::size_t combine(std::size_t a, std::size_t b) noexcept {
            // NOLINTNEXTLINE(*-magic-numbers)
            return (a ^= b + 0x9e3779b97f4a7c15LLU + (a << 12) + (a >> 4));
        }
    };

    struct hash_composer final : private hash_composer_traits<> {
        std::size_t hash{fnv1a_hash_traits<>::prime};

        constexpr operator std::size_t() const noexcept {
            return hash;
        }

        constexpr hash_composer operator<<(std::string_view value) noexcept {
            hash = combine(hash, fnv1a_hash(value.data(), value.size()));
            return *this;
        }

        template < typename Pointer >
            requires std::is_pointer_v<Pointer>
        constexpr hash_composer operator<<(Pointer value) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            hash = combine(hash, reinterpret_cast<std::uintptr_t>(value));
            return *this;
        }

        template < typename Integral >
            requires(std::is_integral_v<Integral> && sizeof(Integral) <= sizeof(std::size_t))
        constexpr hash_composer operator<<(Integral value) noexcept {
            hash = combine(hash, static_cast<std::size_t>(value));
            return *this;
        }

        template < typename Enum >
            requires(std::is_enum_v<Enum> && sizeof(std::underlying_type_t<Enum>) <= sizeof(std::size_t))
        constexpr hash_composer operator<<(Enum value) noexcept {
            hash = combine(hash, static_cast<std::size_t>(value));
            return *this;
        }

        template < typename Enum >
            requires(std::is_enum_v<Enum> && sizeof(std::underlying_type_t<Enum>) <= sizeof(std::size_t))
        constexpr hash_composer operator<<(bitflags<Enum> value) noexcept {
            hash = combine(hash, static_cast<std::size_t>(value.as_raw()));
            return *this;
        }
    };
}
