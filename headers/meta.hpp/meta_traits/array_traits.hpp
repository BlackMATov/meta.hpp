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
    template < array_kind Array >
    struct array_traits {
        static constexpr std::size_t extent{std::extent_v<Array>};

        using data_type = std::remove_extent_t<Array>;

        static bitflags<array_flags> make_flags() noexcept {
            bitflags<array_flags> flags;
            if ( std::is_bounded_array_v<Array> ) flags.set(array_flags::is_bounded);
            if ( std::is_unbounded_array_v<Array> ) flags.set(array_flags::is_unbounded);
            return flags;
        }
    };
}
