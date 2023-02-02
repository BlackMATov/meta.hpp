/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"
#include "type_list.hpp"

namespace meta_hpp::detail
{
    class type_id final {
    public:
        template < typename T >
        explicit type_id(type_list<T>) noexcept
        : id_{type_to_id<T>()} {}

        type_id(type_id&&) = default;
        type_id(const type_id&) = default;

        type_id& operator=(type_id&&) = default;
        type_id& operator=(const type_id&) = default;

        ~type_id() = default;

        void swap(type_id& other) noexcept {
            std::swap(id_, other.id_);
        }

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return std::hash<underlying_type>{}(id_);
        }

        [[nodiscard]] bool operator==(type_id other) const noexcept {
            return id_ == other.id_;
        }

        [[nodiscard]] std::strong_ordering operator<=>(type_id other) const noexcept {
            return id_ <=> other.id_;
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

    inline void swap(type_id& l, type_id& r) noexcept { l.swap(r); }
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
