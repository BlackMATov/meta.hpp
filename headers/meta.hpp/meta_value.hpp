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
        value() = default;

        template < typename T >
        value(T&& value)
        : raw_{std::forward<T>(value)} {}

        template < typename T, typename... Args >
        value(std::in_place_type_t<T>, Args&&... args)
        : raw_{std::in_place_type<T>, std::forward<Args>(args)...} {}

        template < typename T >
        auto cast() const {
            return std::any_cast<T>(raw_);
        }

        template < typename T >
        auto try_cast() noexcept {
            return std::any_cast<T>(&raw_);
        }

        template < typename T >
        auto try_cast() const noexcept {
            return std::any_cast<T>(&raw_);
        }
    public:
        bool to_bool() const { return cast<bool>(); }
        int to_int() const { return cast<int>(); }
        float to_float() const { return cast<float>(); }
        double to_double() const { return cast<double>(); }
        std::string to_string() const { return cast<std::string>(); }

        std::int8_t to_int8() const { return cast<std::int8_t>(); }
        std::int16_t to_int16() const { return cast<std::int16_t>(); }
        std::int32_t to_int32() const { return cast<std::int32_t>(); }
        std::int64_t to_int64() const { return cast<std::int64_t>(); }

        std::uint8_t to_uint8() const { return cast<std::uint8_t>(); }
        std::uint16_t to_uint16() const { return cast<std::uint16_t>(); }
        std::uint32_t to_uint32() const { return cast<std::uint32_t>(); }
        std::uint64_t to_uint64() const { return cast<std::uint64_t>(); }

        std::size_t to_size_t() const { return cast<std::size_t>(); }
        std::ptrdiff_t to_ptrdiff_t() const { return cast<std::ptrdiff_t>(); }
    private:
        std::any raw_;
    };
}
