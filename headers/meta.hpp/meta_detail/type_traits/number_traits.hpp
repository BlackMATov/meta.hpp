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
    enum class number_flags : std::uint8_t {
        is_signed = 1 << 0,
        is_unsigned = 1 << 1,
        is_integral = 1 << 2,
        is_floating_point = 1 << 3,
    };

    using number_bitflags = bitflags<number_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(number_flags)
}

namespace meta_hpp::detail
{
    template < number_kind Number >
    struct number_traits {
        static constexpr std::size_t size{sizeof(Number)};
        static constexpr std::size_t align{alignof(Number)};

        [[nodiscard]] static constexpr number_bitflags make_flags() noexcept {
            number_bitflags flags{};

            if constexpr ( std::is_signed_v<Number> ) {
                flags.set(number_flags::is_signed);
            }

            if constexpr ( std::is_unsigned_v<Number> ) {
                flags.set(number_flags::is_unsigned);
            }

            if constexpr ( std::is_integral_v<Number> ) {
                flags.set(number_flags::is_integral);
            }

            if constexpr ( std::is_floating_point_v<Number> ) {
                flags.set(number_flags::is_floating_point);
            }

            return flags;
        }
    };
}
