/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_details.hpp"
#include "meta_states.hpp"
#include "meta_uresult.hpp"
#include "meta_uvalue.hpp"

namespace meta_hpp
{
    template < typename... Args >
    uvalue invoke(const function& function, Args&&... args);

    template < typename... Args >
    uresult try_invoke(const function& function, Args&&... args);

    template < function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args);

    template < function_pointer_kind Function, typename... Args >
    uresult try_invoke(Function function_ptr, Args&&... args);

    //

    template < typename Iter >
    uvalue invoke_variadic(const function& function, Iter first, Iter last);

    template < typename Iter >
    uresult try_invoke_variadic(const function& function, Iter first, Iter last);

    template < function_pointer_kind Function, typename Iter >
    uvalue invoke_variadic(Function function_ptr, Iter first, Iter last);

    template < function_pointer_kind Function, typename Iter >
    uresult try_invoke_variadic(Function function_ptr, Iter first, Iter last);
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance);

    template < typename Instance >
    uresult try_invoke(const member& member, Instance&& instance);

    template < member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance);

    template < member_pointer_kind Member, typename Instance >
    uresult try_invoke(Member member_ptr, Instance&& instance);
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args);

    template < typename Instance, typename... Args >
    uresult try_invoke(const method& method, Instance&& instance, Args&&... args);

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args);

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args);

    //

    template < typename Instance, typename Iter >
    uvalue invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter >
    uresult try_invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last);

    template < method_pointer_kind Method, typename Instance, typename Iter >
    uvalue invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last);

    template < method_pointer_kind Method, typename Instance, typename Iter >
    uresult try_invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last);
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) noexcept;

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) noexcept;

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr) noexcept;

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr, Args&&... args) noexcept;

    template < typename... Args >
    uerror check_invocable_error(const function& function) noexcept;

    template < typename... Args >
    uerror check_invocable_error(const function& function, Args&&... args) noexcept;

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function function_ptr) noexcept;

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function function_ptr, Args&&... args) noexcept;

    //

    template < typename Iter >
    bool is_variadic_invocable_with(const function& function, Iter first, Iter last);

    template < typename Iter, function_pointer_kind Function >
    bool is_variadic_invocable_with(Function function_ptr, Iter first, Iter last);

    template < typename Iter >
    uerror check_variadic_invocable_error(const function& function, Iter first, Iter last);

    template < typename Iter, function_pointer_kind Function >
    uerror check_variadic_invocable_error(Function function_ptr, Iter first, Iter last);
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) noexcept;

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) noexcept;

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr) noexcept;

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr, Instance&& instance) noexcept;

    template < typename Instance >
    uerror check_invocable_error(const member& member) noexcept;

    template < typename Instance >
    uerror check_invocable_error(const member& member, Instance&& instance) noexcept;

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member member_ptr) noexcept;

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member member_ptr, Instance&& instance) noexcept;
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) noexcept;

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr, Instance&& instance, Args&&... args) noexcept;

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method) noexcept;

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method, Instance&& instance, Args&&... args) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method method_ptr) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method method_ptr, Instance&& instance, Args&&... args) noexcept;

    //

    template < typename Instance, typename Iter >
    bool is_variadic_invocable_with(const method& method, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter, method_pointer_kind Method >
    bool is_variadic_invocable_with(Method method_ptr, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter >
    uerror check_variadic_invocable_error(const method& method, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter, method_pointer_kind Method >
    uerror check_variadic_invocable_error(Method method_ptr, Instance&& instance, Iter first, Iter last);
}
