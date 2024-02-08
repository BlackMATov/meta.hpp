/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

#include "../type_kinds.hpp"

namespace meta_hpp::detail
{
    enum class destructor_flags : std::uint8_t {
        is_noexcept = 1 << 0,
        is_virtual = 1 << 1,
    };

    using destructor_bitflags = bitflags<destructor_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(destructor_flags)
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct destructor_traits;

    template < typename Class >
    concept class_destructor_kind //
        = class_kind<Class> && std::is_destructible_v<Class>;
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

            if constexpr ( std::has_virtual_destructor_v<Class> ) {
                flags.set(destructor_flags::is_virtual);
            }

            return flags;
        }
    };
}
