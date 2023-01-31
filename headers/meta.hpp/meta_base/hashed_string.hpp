/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < std::size_t SizeBytes = sizeof(std::size_t) >
    struct fnv1a_hash_traits;

    template <>
    struct fnv1a_hash_traits<4> { // NOLINT(*-magic-numbers)
        using underlying_type = std::uint32_t;
        static inline constexpr underlying_type prime{16777619U};
        static inline constexpr underlying_type offset{2166136261U};
    };

    template <>
    struct fnv1a_hash_traits<8> { // NOLINT(*-magic-numbers)
        using underlying_type = std::uint64_t;
        static inline constexpr underlying_type prime{1099511628211U};
        static inline constexpr underlying_type offset{14695981039346656037U};
    };

    constexpr std::size_t fnv1a_hash(std::string_view str) noexcept {
        std::size_t hash{fnv1a_hash_traits<>::offset};
        for ( char ch : str ) {
            hash ^= static_cast<std::size_t>(ch);
            hash *= fnv1a_hash_traits<>::prime;
        }
        return hash;
    }
}

namespace meta_hpp::detail
{
    class hashed_string final {
    public:
        hashed_string() = default;
        ~hashed_string() = default;

        hashed_string(hashed_string&&) = default;
        hashed_string(const hashed_string&) = default;

        hashed_string& operator=(hashed_string&&) = default;
        hashed_string& operator=(const hashed_string&) = default;

        constexpr hashed_string(const char* str) noexcept
        : hash_{fnv1a_hash(str)} {}

        constexpr hashed_string(std::string_view str) noexcept
        : hash_{fnv1a_hash(str)} {}

        hashed_string(const std::string& str) noexcept
        : hash_{fnv1a_hash(str)} {}

        constexpr void swap(hashed_string& other) noexcept {
            std::swap(hash_, other.hash_);
        }

        [[nodiscard]] constexpr std::size_t get_hash() const noexcept {
            return hash_;
        }
    private:
        std::size_t hash_{fnv1a_hash("")};
    };

    constexpr void swap(hashed_string& l, hashed_string& r) noexcept {
        l.swap(r);
    }

    [[nodiscard]] constexpr bool operator<(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() < r.get_hash();
    }

    [[nodiscard]] constexpr bool operator==(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() == r.get_hash();
    }

    [[nodiscard]] constexpr bool operator!=(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() != r.get_hash();
    }
}

namespace std
{
    template <>
    struct hash<meta_hpp::detail::hashed_string> {
        size_t operator()(meta_hpp::detail::hashed_string hs) const noexcept {
            return hs.get_hash();
        }
    };
}
