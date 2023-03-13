/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_states.hpp"
#include "meta_uvalue.hpp"

namespace meta_hpp
{
    template < typename... Args >
    uvalue invoke(const function& function, Args&&... args);

    template < typename... Args >
    uresult try_invoke(const function& function, Args&&... args);

    template < detail::function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args);

    template < detail::function_pointer_kind Function, typename... Args >
    uresult try_invoke(Function function_ptr, Args&&... args);
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance);

    template < typename Instance >
    uresult try_invoke(const member& member, Instance&& instance);

    template < detail::member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance);

    template < detail::member_pointer_kind Member, typename Instance >
    uresult try_invoke(Member member_ptr, Instance&& instance);
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args);

    template < typename Instance, typename... Args >
    uresult try_invoke(const method& method, Instance&& instance, Args&&... args);

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args);

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args);
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) noexcept;

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) noexcept;

    template < typename... Args, detail::function_pointer_kind Function >
    bool is_invocable_with(Function) noexcept;

    template < typename... Args, detail::function_pointer_kind Function >
    bool is_invocable_with(Function, Args&&... args) noexcept;
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) noexcept;

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) noexcept;

    template < typename Instance, detail::member_pointer_kind Member >
    bool is_invocable_with(Member) noexcept;

    template < typename Instance, detail::member_pointer_kind Member >
    bool is_invocable_with(Member, Instance&& instance) noexcept;
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) noexcept;

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept;

    template < typename Instance, typename... Args, detail::method_pointer_kind Method >
    bool is_invocable_with(Method) noexcept;

    template < typename Instance, typename... Args, detail::method_pointer_kind Method >
    bool is_invocable_with(Method, Instance&& instance, Args&&... args) noexcept;
}
