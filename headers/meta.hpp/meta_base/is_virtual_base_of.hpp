/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename From, typename To >
        constexpr bool is_virtual_base_of_impl(...) noexcept {
            return false;
        }

        template <
            typename From,
            typename To,
            decltype(static_cast<const volatile To*>(std::declval<const volatile From*>())) = nullptr >
        constexpr bool is_virtual_base_of_impl(int) noexcept {
            return true;
        }
    }

    // clang-format off

    template < typename Base, typename Derived >
    struct is_virtual_base_of : std::integral_constant<bool,
          std::is_base_of_v<Base, Derived> &&
          impl::is_virtual_base_of_impl<Derived, Base>(0) &&
          !impl::is_virtual_base_of_impl<Base, Derived>(0)> {};

    // clang-format on

    template < typename Base, typename Derived >
    inline constexpr bool is_virtual_base_of_v = is_virtual_base_of<Base, Derived>::value;
}
