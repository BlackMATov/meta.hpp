/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_value.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct less_traits;

    template < typename T >
    concept has_less_traits = requires(const T& l, const T& r) {
        { less_traits<T>{}(l, r) } -> stdex::convertible_to<bool>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(const T& l, const T& r) {
            { std::less<>{}(l, r) } -> stdex::convertible_to<bool>;
        }
    struct less_traits<T> {
        bool operator()(const T& l, const T& r) const {
            return std::less<>{}(l, r);
        }
    };
}
