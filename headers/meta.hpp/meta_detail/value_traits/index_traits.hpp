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
    struct index_traits;

    template < typename T >
    concept has_index_traits //
        = requires(const T& v, std::size_t i) {
              { index_traits<T>{}(v, i) } -> std::convertible_to<uvalue>;
          };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(const T& v, std::size_t i) { uvalue{v[i]}; }
    struct index_traits<T> {
        uvalue operator()(const T& v, std::size_t i) const {
            return uvalue{v[i]};
        }
    };
}
