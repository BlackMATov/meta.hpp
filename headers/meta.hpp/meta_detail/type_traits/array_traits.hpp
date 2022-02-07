/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class array_flags : std::uint32_t {
        is_bounded = 1 << 0,
        is_unbounded = 1 << 1,
    };

    ENUM_HPP_OPERATORS_DECL(array_flags)
    using array_bitflags = bitflags<array_flags>;
}

namespace meta_hpp::detail
{
    template < array_kind Array >
    struct array_traits {
        static constexpr std::size_t extent{std::extent_v<Array>};

        using data_type = std::remove_extent_t<Array>;

        [[nodiscard]] static constexpr array_bitflags make_flags() noexcept {
            array_bitflags flags{};

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
