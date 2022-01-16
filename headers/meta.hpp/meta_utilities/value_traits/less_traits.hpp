/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct value_less_traits;

    template < typename T >
    concept has_value_less_traits = requires(const T& v) {
        { value_less_traits<T>{}(v, v) } -> stdex::convertible_to<bool>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(const T& v) {
            { v < v } -> stdex::convertible_to<bool>;
        }
    struct value_less_traits<T> {
        bool operator()(const T& l, const T& r) const {
            return l < r;
        }
    };
}
