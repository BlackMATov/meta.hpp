/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_utilities.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct value_equals_traits;

    template < typename T >
    concept has_value_equals_traits = requires(const T& l, const T& r) {
        { value_equals_traits<T>{}(l, r) } -> stdex::convertible_to<bool>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(const T& l, const T& r) {
            { l == r } -> stdex::convertible_to<bool>;
        }
    struct value_equals_traits<T> {
        bool operator()(const T& l, const T& r) const {
            return l == r;
        }
    };
}
