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

    template < detail::function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args);
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance);

    template < detail::member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance);
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args);

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args);
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function);

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args);

    template < detail::function_pointer_kind Function, typename... Args >
    bool is_invocable_with();

    template < detail::function_pointer_kind Function, typename... Args >
    bool is_invocable_with(Args&&... args);
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member);

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance);

    template < detail::member_pointer_kind Member, typename Instance >
    bool is_invocable_with();

    template < detail::member_pointer_kind Member, typename Instance >
    bool is_invocable_with(Instance&& instance);
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method);

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args);

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    bool is_invocable_with();

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    bool is_invocable_with(Instance&& instance, Args&&... args);
}
