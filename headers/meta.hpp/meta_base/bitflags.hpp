/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename Enum >
    class bitflags final {
        static_assert(std::is_enum_v<Enum>);

    public:
        using enum_type = Enum;
        using underlying_type = std::underlying_type_t<Enum>;

        bitflags() = default;
        ~bitflags() = default;

        bitflags(bitflags&&) = default;
        bitflags(const bitflags&) = default;

        bitflags& operator=(bitflags&&) = default;
        bitflags& operator=(const bitflags&) = default;

        constexpr bitflags(enum_type flags)
        : flags_(static_cast<underlying_type>(flags)) {}

        constexpr explicit bitflags(underlying_type flags)
        : flags_(flags) {}

        constexpr void swap(bitflags& other) noexcept {
            using std::swap;
            swap(flags_, other.flags_);
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return !!flags_;
        }

        [[nodiscard]] constexpr underlying_type as_raw() const noexcept {
            return flags_;
        }

        [[nodiscard]] constexpr enum_type as_enum() const noexcept {
            return static_cast<enum_type>(flags_);
        }

        [[nodiscard]] constexpr bool has(bitflags flags) const noexcept {
            return flags.flags_ == (flags_ & flags.flags_);
        }

        constexpr bitflags& set(bitflags flags) noexcept {
            flags_ |= flags.flags_;
            return *this;
        }

        constexpr bitflags& toggle(bitflags flags) noexcept {
            flags_ ^= flags.flags_;
            return *this;
        }

        constexpr bitflags& clear(bitflags flags) noexcept {
            flags_ &= ~flags.flags_;
            return *this;
        }

    private:
        underlying_type flags_{};
    };

    template < typename Enum >
    constexpr void swap(bitflags<Enum>& l, bitflags<Enum>& r) noexcept {
        l.swap(r);
    }
}

namespace std
{
    template < typename Enum >
    struct hash<meta_hpp::detail::bitflags<Enum>> {
        size_t operator()(meta_hpp::detail::bitflags<Enum> bf) const noexcept {
            return hash<Enum>{}(bf.as_enum());
        }
    };
}

namespace meta_hpp::detail
{
#define META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(op) \
    template < typename Enum > \
    constexpr bool operator op(Enum l, bitflags<Enum> r) noexcept { \
        return l op r.as_enum(); \
    } \
    template < typename Enum > \
    constexpr bool operator op(bitflags<Enum> l, Enum r) noexcept { \
        return l.as_enum() op r; \
    } \
    template < typename Enum > \
    constexpr bool operator op(std::underlying_type_t<Enum> l, bitflags<Enum> r) noexcept { \
        return l op r.as_raw(); \
    } \
    template < typename Enum > \
    constexpr bool operator op(bitflags<Enum> l, std::underlying_type_t<Enum> r) noexcept { \
        return l.as_raw() op r; \
    } \
    template < typename Enum > \
    constexpr bool operator op(bitflags<Enum> l, bitflags<Enum> r) noexcept { \
        return l.as_raw() op r.as_raw(); \
    }
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(<)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(>)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(<=)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(>=)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(==)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(!=)
#undef META_HPP_DETAIL_DEFINE_BINARY_OPERATOR
}

namespace meta_hpp::detail
{
    template < typename Enum >
    constexpr bitflags<Enum> operator~(bitflags<Enum> l) noexcept {
        return static_cast<Enum>(~l.as_raw());
    }

#define META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(op) \
    template < typename Enum > \
    constexpr bitflags<Enum> operator op(Enum l, bitflags<Enum> r) noexcept { \
        return bitflags{l} op r; \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum> operator op(bitflags<Enum> l, Enum r) noexcept { \
        return l op bitflags<Enum>{r}; \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum> operator op(bitflags<Enum> l, bitflags<Enum> r) noexcept { \
        return static_cast<Enum>(l.as_raw() op r.as_raw()); \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum>& operator op##=(bitflags<Enum>& l, Enum r) noexcept { \
        return l = l op bitflags<Enum>{r}; \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum>& operator op##=(bitflags<Enum>& l, bitflags<Enum> r) noexcept { \
        return l = l op r; \
    }
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(|)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(&)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(^)
#undef META_HPP_DETAIL_DEFINE_BINARY_OPERATOR
}

//
// META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL
//

#define META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(Enum) \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator~[[maybe_unused]] (Enum l) noexcept { \
        return ~::meta_hpp::detail::bitflags<Enum>(l); \
    } \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator| [[maybe_unused]] (Enum l, Enum r) noexcept { \
        return ::meta_hpp::detail::bitflags<Enum>(l) | ::meta_hpp::detail::bitflags<Enum>(r); \
    } \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator& [[maybe_unused]] (Enum l, Enum r) noexcept { \
        return ::meta_hpp::detail::bitflags<Enum>(l) & ::meta_hpp::detail::bitflags<Enum>(r); \
    } \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator^ [[maybe_unused]] (Enum l, Enum r) noexcept { \
        return ::meta_hpp::detail::bitflags<Enum>(l) ^ ::meta_hpp::detail::bitflags<Enum>(r); \
    }
