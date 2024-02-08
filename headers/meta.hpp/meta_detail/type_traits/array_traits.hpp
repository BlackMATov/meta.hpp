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
    enum class array_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
        is_bounded = 1 << 2,
        is_unbounded = 1 << 3,
    };

    using array_bitflags = bitflags<array_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(array_flags)
}

namespace meta_hpp::detail
{
    template < array_kind Array >
    struct array_traits {
        static constexpr std::size_t extent{std::extent_v<Array>};

        using cv_data_type = std::remove_extent_t<Array>;
        inline static constexpr bool is_readonly = std::is_const_v<cv_data_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_data_type>;

        using data_type = std::remove_cv_t<cv_data_type>;

        [[nodiscard]] static constexpr array_bitflags make_flags() noexcept {
            array_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(array_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(array_flags::is_volatile);
            }

            if constexpr ( std::is_bounded_array_v<Array> ) {
                flags.set(array_flags::is_bounded);
            }

            if constexpr ( std::is_unbounded_array_v<Array> ) {
                flags.set(array_flags::is_unbounded);
            }

            return flags;
        }
    };
}
