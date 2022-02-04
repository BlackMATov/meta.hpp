/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class enum_flags : std::uint32_t {
        is_scoped = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(enum_flags)
}

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    struct enum_traits {
        using underlying_type = std::underlying_type_t<Enum>;

        [[nodiscard]] static constexpr bitflags<enum_flags> make_flags() noexcept {
            bitflags<enum_flags> flags;

            if constexpr ( !std::is_convertible_v<Enum, underlying_type> ) {
                flags.set(enum_flags::is_scoped);
            }

            return flags;
        }
    };
}
