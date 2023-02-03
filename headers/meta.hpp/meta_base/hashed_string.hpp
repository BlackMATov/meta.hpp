/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"
#include "fnv1a_hash.hpp"

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

        constexpr hashed_string(std::string_view str) noexcept
        : hash_{fnv1a_hash(str.data(), str.size())} {}

        constexpr void swap(hashed_string& other) noexcept {
            std::swap(hash_, other.hash_);
        }

        [[nodiscard]] constexpr std::size_t get_hash() const noexcept {
            return hash_;
        }

        [[nodiscard]] constexpr bool operator==(hashed_string other) const noexcept {
            return hash_ == other.hash_;
        }

        [[nodiscard]] constexpr std::strong_ordering operator<=>(hashed_string other) const noexcept {
            return hash_ <=> other.hash_;
        }
    private:
        std::size_t hash_{fnv1a_hash("", 0)};
    };

    constexpr void swap(hashed_string& l, hashed_string& r) noexcept {
        l.swap(r);
    }

    [[nodiscard]] constexpr bool operator==(hashed_string l, std::string_view r) noexcept {
        return l == hashed_string{r};
    }

    [[nodiscard]] constexpr std::strong_ordering operator<=>(hashed_string l, std::string_view r) noexcept {
        return l <=> hashed_string{r};
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
