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
    enum class member_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
    };

    using member_bitflags = bitflags<member_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(member_flags)
}

namespace meta_hpp::detail
{
    template < member_pointer_kind Member >
    struct member_traits;

    template < typename Member, typename Class >
    concept class_member_pointer_kind                      //
        = class_kind<Class> && member_pointer_kind<Member> //
       && std::is_same_v<Class, typename member_traits<Member>::class_type>;
}

namespace meta_hpp::detail
{
    template < typename V, typename C >
    struct member_traits<V C::*> {
        using cv_value_type = V;
        inline static constexpr bool is_readonly = std::is_const_v<cv_value_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_value_type>;

        using class_type = C;
        using value_type = std::remove_cv_t<cv_value_type>;

        [[nodiscard]] static constexpr member_bitflags make_flags() noexcept {
            member_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(member_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(member_flags::is_volatile);
            }

            return flags;
        }
    };
}
