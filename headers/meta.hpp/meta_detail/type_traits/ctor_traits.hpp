/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class ctor_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(ctor_flags)
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct ctor_traits {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = Class;
        using argument_types = type_list<Args...>;

        [[nodiscard]] static constexpr bitflags<ctor_flags> make_flags() noexcept {
            bitflags<ctor_flags> flags;

            if constexpr ( std::is_nothrow_constructible_v<Class, Args...> ) {
                flags.set(ctor_flags::is_noexcept);
            }

            return flags;
        }
    };
}