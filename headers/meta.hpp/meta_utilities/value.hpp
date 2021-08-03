/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    class value final {
    public:
        value() = delete;

        value(value&&) = default;
        value& operator=(value&&) = default;

        value(const value&) = default;
        value& operator=(const value&) = default;

        template < typename T, typename Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0 >
        value(T&& val)
        : raw_{std::forward<T>(val)} {}

        template < typename T, typename Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0 >
        value& operator=(T&& val) {
            value{std::forward<T>(val)}.swap(*this);
            return *this;
        }

        void swap(value& other) noexcept {
            using std::swap;
            swap(raw_, other.raw_);
        }
    private:
        std::any raw_{};
    };

    inline void swap(value& l, value& r) noexcept {
        l.swap(r);
    }
}
