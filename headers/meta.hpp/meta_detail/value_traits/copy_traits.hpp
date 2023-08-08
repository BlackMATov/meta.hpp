/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct copy_traits;

    template < typename T >
    concept has_copy_traits //
        = requires(const T& v) {
              { copy_traits<T>{}(v) } -> std::convertible_to<uvalue>;
          };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct copy_traits<T> {
        uvalue operator()(const T& v) const {
            return uvalue{v};
        }
    };
}
