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
    enum class constructor_flags : std::uint8_t {
        is_noexcept = 1 << 0,
    };

    using constructor_bitflags = bitflags<constructor_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(constructor_flags)
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_traits;

    template < typename Class, typename... Args >
    concept class_constructor_kind //
        = class_kind<Class> && std::is_constructible_v<Class, Args...>;
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_traits {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = Class;
        using argument_types = type_list<std::remove_cv_t<Args>...>;

        [[nodiscard]] static constexpr constructor_bitflags make_flags() noexcept {
            constructor_bitflags flags{};

            if constexpr ( std::is_nothrow_constructible_v<Class, Args...> ) {
                flags.set(constructor_flags::is_noexcept);
            }

            return flags;
        }
    };
}
