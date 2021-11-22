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
    template < number_kind Number >
    struct number_traits {
        static constexpr std::size_t size{sizeof(Number)};

        static bitflags<number_flags> make_flags() noexcept {
            bitflags<number_flags> flags;
            if ( std::is_signed_v<Number> ) flags.set(number_flags::is_signed);
            if ( std::is_unsigned_v<Number> ) flags.set(number_flags::is_unsigned);
            if ( std::is_integral_v<Number> ) flags.set(number_flags::is_integral);
            if ( std::is_floating_point_v<Number> ) flags.set(number_flags::is_floating_point);
            return flags;
        }
    };
}
