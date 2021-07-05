/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

namespace meta_hpp
{
    class value {
    public:
        value() = delete;

        value(value&&) = default;
        value(const value&) = default;

        value& operator=(value&&) = default;
        value& operator=(const value&) = default;

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>> >
        value(T&& val)
        : raw_{std::forward<T>(val)}
        , fid_{get_type_family_id<U>()} {}

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>> >
        value& operator=(T&& val) {
            value{std::forward<T>(val)}.swap(*this);
            return *this;
        }

        template < typename T, typename... Args >
        explicit value(std::in_place_type_t<T>, Args&&... args)
        : raw_{std::in_place_type<T>, std::forward<Args>(args)...}
        , fid_{get_type_family_id<T>()} {}

        template < typename T, typename U, typename... Args >
        explicit value(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args)
        : raw_{std::in_place_type<T>, ilist, std::forward<Args>(args)...}
        , fid_{get_type_family_id<T>()} {}

        template < typename T >
        T cast() && {
            return std::any_cast<T>(std::move(raw_));
        }

        template < typename T >
        T cast() & {
            return std::any_cast<T>(raw_);
        }

        template < typename T >
        T cast() const & {
            return std::any_cast<T>(raw_);
        }

        template < typename T >
        std::add_pointer_t<T>
        try_cast() noexcept {
            return std::any_cast<T>(&raw_);
        }

        template < typename T >
        std::add_pointer_t<std::add_const_t<T>>
        try_cast() const noexcept {
            return std::any_cast<T>(&raw_);
        }

        family_id fid() const noexcept {
            return fid_;
        }

        void swap(value& other) noexcept {
            using std::swap;
            swap(raw_, other.raw_);
            swap(fid_, other.fid_);
        }
    public:
        bool to_bool() const { return cast<bool>(); }
        int to_int() const { return cast<int>(); }
        unsigned to_uint() const { return cast<unsigned>(); }
        float to_float() const { return cast<float>(); }
        double to_double() const { return cast<double>(); }
        std::string to_string() const { return cast<std::string>(); }

        std::int8_t to_int8() const { return cast<std::int8_t>(); }
        std::int16_t to_int16() const { return cast<std::int16_t>(); }
        std::int32_t to_int32() const { return cast<std::int32_t>(); }
        std::int64_t to_int64() const { return cast<std::int64_t>(); }
        std::ptrdiff_t to_ptrdiff_t() const { return cast<std::ptrdiff_t>(); }
        std::intptr_t to_intptr_t() const { return cast<std::intptr_t>(); }

        std::uint8_t to_uint8() const { return cast<std::uint8_t>(); }
        std::uint16_t to_uint16() const { return cast<std::uint16_t>(); }
        std::uint32_t to_uint32() const { return cast<std::uint32_t>(); }
        std::uint64_t to_uint64() const { return cast<std::uint64_t>(); }
        std::size_t to_size_t() const { return cast<std::size_t>(); }
        std::uintptr_t to_uintptr_t() const { return cast<std::uintptr_t>(); }
    private:
        std::any raw_;
        family_id fid_;
    };

    inline void swap(value& l, value& r) noexcept {
        l.swap(r);
    }
}
