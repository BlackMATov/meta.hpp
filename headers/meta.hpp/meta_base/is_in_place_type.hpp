/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct is_in_place_type : std::false_type {};

    template < typename U >
    struct is_in_place_type<std::in_place_type_t<U>> : std::true_type {};

    template < typename T >
    inline constexpr bool is_in_place_type_v = is_in_place_type<T>::value;
}
