/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename C, typename R, typename... Args >
    constexpr auto select_const(R (C::*func)(Args...) const) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_const(R (C::*func)(Args...) const noexcept) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_non_const(R (C::*func)(Args...)) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_non_const(R (C::*func)(Args...) noexcept) -> decltype(func) {
        return func;
    }

    template < typename Signature >
    constexpr auto select_overload(Signature* func) noexcept -> decltype(func) {
        return func;
    }

    template < typename Signature, typename C >
    constexpr auto select_overload(Signature C::*func) noexcept -> decltype(func) {
        return func;
    }
}
