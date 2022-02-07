/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class dtor_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(dtor_flags)
    using dtor_bitflags = bitflags<dtor_flags>;
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct dtor_traits {
        using class_type = Class;

        [[nodiscard]] static constexpr dtor_bitflags make_flags() noexcept {
            dtor_bitflags flags{};

            if constexpr ( std::is_nothrow_destructible_v<Class> ) {
                flags.set(dtor_flags::is_noexcept);
            }

            return flags;
        }
    };
}
