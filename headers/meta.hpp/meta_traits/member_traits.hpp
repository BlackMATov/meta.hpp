/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_traits.hpp"

namespace meta_hpp::detail
{
    template < member_kind Member >
    struct member_traits;

    template < typename V, typename C >
    struct member_traits<V C::*> {
        using class_type = C;
        using value_type = V;

        [[nodiscard]] static constexpr bitflags<member_flags> make_flags() noexcept {
            return {};
        }
    };
}
