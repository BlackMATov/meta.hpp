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
    enum class reference_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
        is_lvalue = 1 << 2,
        is_rvalue = 1 << 3,
    };

    using reference_bitflags = bitflags<reference_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(reference_flags)
}

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    struct reference_traits {
        using cv_data_type = std::remove_reference_t<Reference>;
        inline static constexpr bool is_readonly = std::is_const_v<cv_data_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_data_type>;

        using data_type = std::remove_cv_t<cv_data_type>;

        [[nodiscard]] static constexpr reference_bitflags make_flags() noexcept {
            reference_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(reference_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(reference_flags::is_volatile);
            }

            if constexpr ( std::is_lvalue_reference_v<Reference> ) {
                flags.set(reference_flags::is_lvalue);
            }

            if constexpr ( std::is_rvalue_reference_v<Reference> ) {
                flags.set(reference_flags::is_rvalue);
            }

            return flags;
        }
    };
}
