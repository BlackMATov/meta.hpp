/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class reference_flags : std::uint32_t {
        is_readonly = 1 << 0,
        is_lvalue = 1 << 1,
        is_rvalue = 1 << 2,
    };

    ENUM_HPP_OPERATORS_DECL(reference_flags)
}

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    struct reference_traits {
        using data_type = std::remove_reference_t<Reference>;

        [[nodiscard]] static constexpr bitflags<reference_flags> make_flags() noexcept {
            bitflags<reference_flags> flags;

            if constexpr ( std::is_const_v<data_type> ) {
                flags.set(reference_flags::is_readonly);
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