/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_traits.hpp"

namespace meta_hpp::detail
{
    template < method_kind Method >
    struct method_traits;

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = C;
        using return_type = R;
        using qualified_type = C;
        using argument_types = type_list<Args...>;

        static bitflags<method_flags> make_flags() noexcept {
            return {};
        }

        static std::vector<any_type> make_argument_types() {
            return { resolve_type<Args>()... };
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_const;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) &> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) & noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const &> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_const | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const & noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) &&> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) && noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const &&> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_const | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const && noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };

    //

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile const> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_const;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile const noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_const | method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile &> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile & noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile const &> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_const | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile const & noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_const | method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile &&> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile && noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile const &&> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_const | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) volatile const && noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&&;
        static bitflags<method_flags> make_flags() noexcept {
            return method_flags::is_volatile | method_flags::is_const | method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };
}
