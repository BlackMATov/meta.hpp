/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <type_traits>

namespace meta_hpp::stdex
{
    template < typename T, typename U >
    concept same_as =
        std::is_same_v<T, U> &&
        std::is_same_v<U, T>;

    template < typename Derived, typename Base >
    concept derived_from =
        std::is_base_of_v<Base, Derived> &&
        std::is_convertible_v<const volatile Derived*, const volatile Base*>;

    template < typename From, typename To >
    concept convertible_to =
        std::is_convertible_v<From, To> &&
        requires { static_cast<To>(std::declval<From>()); };

    template < typename T >
    concept destructible =
        std::is_nothrow_destructible_v<T>;

    template < typename T, typename... Args >
    concept constructible_from =
        destructible<T> &&
        std::is_constructible_v<T, Args...>;

    template < typename T >
    concept move_constructible =
        constructible_from<T, T> &&
        convertible_to<T, T>;

    template<typename T>
    concept copy_constructible =
        move_constructible<T> &&
        constructible_from<T, T&> && convertible_to<T&, T> &&
        constructible_from<T, const T&> && convertible_to<const T&, T> &&
        constructible_from<T, const T> && convertible_to<const T, T>;
}

namespace meta_hpp::stdex
{
    template < typename Enum >
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}
