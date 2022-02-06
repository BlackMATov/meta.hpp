/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class number_flags : std::uint32_t {
        is_signed = 1 << 0,
        is_unsigned = 1 << 1,
        is_integral = 1 << 2,
        is_floating_point = 1 << 3,
    };

    ENUM_HPP_OPERATORS_DECL(number_flags)
}

namespace meta_hpp::detail
{
    template < number_kind Number >
    struct number_traits {
        static constexpr std::size_t size{sizeof(Number)};

        [[nodiscard]] static constexpr bitflags<number_flags> make_flags() noexcept {
            bitflags<number_flags> flags;

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