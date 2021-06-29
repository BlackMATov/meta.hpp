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
    private:
        std::any raw_;
    };
}
