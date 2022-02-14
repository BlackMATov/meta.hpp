/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>

#include "type_list.hpp"

namespace meta_hpp::detail
{
    class type_id final {
    public:
        template < typename T >
        // NOLINTNEXTLINE(readability-named-parameter)
        explicit type_id(type_list<T>) noexcept
        : id_{type_to_id<T>()} {}

        type_id(type_id&&) = default;
        type_id(const type_id&) = default;

        type_id& operator=(type_id&&) = default;
        type_id& operator=(const type_id&) = default;

        ~type_id() = default;

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return std::hash<underlying_type>{}(id_);
        }

        [[nodiscard]] friend bool operator<(type_id l, type_id r) noexcept {
            return l.id_ < r.id_;
        }

        [[nodiscard]] friend bool operator==(type_id l, type_id r) noexcept {
            return l.id_ == r.id_;
        }

        [[nodiscard]] friend bool operator!=(type_id l, type_id r) noexcept {
            return l.id_ != r.id_;
        }
    private:
        using underlying_type = std::uint32_t;
        underlying_type id_{};
    private:
        [[nodiscard]] static underlying_type next() noexcept {
            static std::atomic<underlying_type> id{};
            return ++id;
        }

        template < typename T >
        [[nodiscard]] static underlying_type type_to_id() noexcept {
            static const underlying_type id{next()};
            return id;
        }
    };
}

namespace std
{
    template <>
    struct hash<meta_hpp::detail::type_id> {
        size_t operator()(const meta_hpp::detail::type_id& id) const noexcept {
            return id.get_hash();
        }
    };
}
