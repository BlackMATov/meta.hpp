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
    struct value_ostream_traits;

    template < typename T >
    concept has_value_ostream_traits = requires(std::ostream& is, const T& v) {
        { value_ostream_traits<T>{}(is, v) } -> stdex::convertible_to<std::ostream&>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(std::ostream& os, const T& v) {
            { os << v } -> stdex::convertible_to<std::ostream&>;
        }
    struct value_ostream_traits<T> {
        std::ostream& operator()(std::ostream& is, const T& v) const {
            return is << v;
        }
    };
}
