/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class function_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(function_flags)
    using function_bitflags = bitflags<function_flags>;
}

namespace meta_hpp::detail
{
    template < function_pointer_kind Function >
    struct function_traits;

    template < typename R, typename... Args >
    struct function_traits<R (*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using return_type = R;
        using argument_types = type_list<Args...>;

        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return {};
        }
    };

    template < typename R, typename... Args >
    struct function_traits<R (*)(Args...) noexcept> : function_traits<R (*)(Args...)> {
        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return function_flags::is_noexcept;
        }
    };
}
