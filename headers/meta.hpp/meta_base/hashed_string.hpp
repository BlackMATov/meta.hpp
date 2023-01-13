/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

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

        hashed_string(const char* str) noexcept
        : hash_{std::hash<std::string_view>{}(str)} {}

        hashed_string(std::string_view str) noexcept
        : hash_{std::hash<std::string_view>{}(str)} {}

        hashed_string(const std::string& str) noexcept
        : hash_{std::hash<std::string_view>{}(str)} {}

        void swap(hashed_string& other) noexcept {
            std::swap(hash_, other.hash_);
        }

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return hash_;
        }
    private:
        std::size_t hash_{};
    };

    inline void swap(hashed_string& l, hashed_string& r) noexcept {
        l.swap(r);
    }

    [[nodiscard]] inline bool operator<(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() < r.get_hash();
    }

    [[nodiscard]] inline bool operator==(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() == r.get_hash();
    }

    [[nodiscard]] inline bool operator!=(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() == r.get_hash();
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
