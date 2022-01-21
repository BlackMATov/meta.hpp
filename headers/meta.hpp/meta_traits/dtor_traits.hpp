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
    template < class_kind Class >
    struct dtor_traits {
        using class_type = Class;

        [[nodiscard]] static constexpr bitflags<dtor_flags> make_flags() noexcept {
            bitflags<dtor_flags> flags;

            if constexpr ( std::is_nothrow_destructible_v<Class> ) {
                flags.set(dtor_flags::is_noexcept);
            }

            return flags;
        }
    };
}
