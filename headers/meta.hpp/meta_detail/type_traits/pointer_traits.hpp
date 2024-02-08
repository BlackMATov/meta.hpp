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
    enum class pointer_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
    };

    using pointer_bitflags = bitflags<pointer_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(pointer_flags)
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    struct pointer_traits {
        using cv_data_type = std::remove_pointer_t<Pointer>;
        inline static constexpr bool is_readonly = std::is_const_v<cv_data_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_data_type>;

        using data_type = std::remove_cv_t<cv_data_type>;

        [[nodiscard]] static constexpr pointer_bitflags make_flags() noexcept {
            pointer_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(pointer_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(pointer_flags::is_volatile);
            }

            return flags;
        }
    };
}
