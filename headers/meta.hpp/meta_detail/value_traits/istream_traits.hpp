/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct istream_traits;

    template < typename T >
    concept has_istream_traits = requires(std::istream& is, T& v) {
        { istream_traits<T>{}(is, v) } -> std::convertible_to<std::istream&>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(std::istream& is, T& v) {
            { is >> v } -> std::convertible_to<std::istream&>;
        }
    struct istream_traits<T> {
        std::istream& operator()(std::istream& is, T& v) const {
            return is >> v;
        }
    };
}
