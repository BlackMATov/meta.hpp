/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class member_flags : std::uint32_t {
        is_readonly = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(member_flags)
    using member_bitflags = bitflags<member_flags>;
}

namespace meta_hpp::detail
{
    template < member_kind Member >
    struct member_traits;

    template < typename V, typename C >
    struct member_traits<V C::*> {
        using class_type = C;
        using value_type = V;

        [[nodiscard]] static constexpr member_bitflags make_flags() noexcept {
            member_bitflags flags{};

            if constexpr ( std::is_const_v<value_type> ) {
                flags.set(member_flags::is_readonly);
            }

            return flags;
        }
    };
}
