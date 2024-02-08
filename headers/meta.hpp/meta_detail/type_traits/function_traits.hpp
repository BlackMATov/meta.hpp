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
    enum class function_flags : std::uint8_t {
        is_noexcept = 1 << 0,
    };

    using function_bitflags = bitflags<function_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(function_flags)
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    struct function_traits;

    template < typename R, typename... Args >
    struct function_traits<R(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using return_type = std::remove_cv_t<R>;
        using argument_types = type_list<std::remove_cv_t<Args>...>;

        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return {};
        }
    };

    template < typename R, typename... Args >
    struct function_traits<R(Args...) noexcept> : function_traits<R(Args...)> {
        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return function_flags::is_noexcept;
        }
    };
}
