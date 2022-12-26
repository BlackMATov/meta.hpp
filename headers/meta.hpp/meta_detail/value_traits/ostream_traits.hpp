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
    struct ostream_traits;

    template < typename T >
    concept has_ostream_traits = requires(std::ostream& os, const T& v) {
        { ostream_traits<T>{}(os, v) } -> std::convertible_to<std::ostream&>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(std::ostream& os, const T& v) {
            { os << v } -> std::convertible_to<std::ostream&>;
        }
    struct ostream_traits<T> {
        std::ostream& operator()(std::ostream& os, const T& v) const {
            return os << v;
        }
    };
}
