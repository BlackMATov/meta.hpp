/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class constructor_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(constructor_flags)
    using constructor_bitflags = bitflags<constructor_flags>;
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_traits {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = Class;
        using argument_types = type_list<Args...>;

        [[nodiscard]] static constexpr constructor_bitflags make_flags() noexcept {
            constructor_bitflags flags{};

            if constexpr ( std::is_nothrow_constructible_v<Class, Args...> ) {
                flags.set(constructor_flags::is_noexcept);
            }

            return flags;
        }
    };
}
