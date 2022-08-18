/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class pointer_flags : std::uint32_t {
        is_readonly = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(pointer_flags)
    using pointer_bitflags = bitflags<pointer_flags>;
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    struct pointer_traits {
        using data_type = std::remove_pointer_t<Pointer>;

        [[nodiscard]] static constexpr pointer_bitflags make_flags() noexcept {
            pointer_bitflags flags{};

            if constexpr ( std::is_const_v<data_type> ) {
                flags.set(pointer_flags::is_readonly);
            }

            return flags;
        }
    };
}
