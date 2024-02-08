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
    enum class method_flags : std::uint8_t {
        is_const = 1 << 0,
        is_noexcept = 1 << 1,
        is_lvalue_qualified = 1 << 2,
        is_rvalue_qualified = 1 << 3,
    };

    using method_bitflags = bitflags<method_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(method_flags)
}

namespace meta_hpp::detail
{
    template < method_pointer_kind Method >
    struct method_traits;

    template < typename Method, typename Class >
    concept class_method_pointer_kind                      //
        = class_kind<Class> && method_pointer_kind<Method> //
       && std::is_same_v<Class, typename method_traits<Method>::class_type>;
}

namespace meta_hpp::detail
{
    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = C;
        using return_type = std::remove_cv_t<R>;
        using qualified_type = C;
        using argument_types = type_list<std::remove_cv_t<Args>...>;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return {};
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...)&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) & noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const & noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) &&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) && noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const&&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const && noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };
}
