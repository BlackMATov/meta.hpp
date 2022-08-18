/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class destructor_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(destructor_flags)
    using destructor_bitflags = bitflags<destructor_flags>;
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct destructor_traits {
        using class_type = Class;

        [[nodiscard]] static constexpr destructor_bitflags make_flags() noexcept {
            destructor_bitflags flags{};

            if constexpr ( std::is_nothrow_destructible_v<Class> ) {
                flags.set(destructor_flags::is_noexcept);
            }

            return flags;
        }
    };
}
