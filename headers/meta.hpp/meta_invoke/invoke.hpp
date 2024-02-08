/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_invoke.hpp"
#include "../meta_states.hpp"
#include "../meta_uresult.hpp"
#include "../meta_uvalue.hpp"

#include "../meta_states/function.hpp"
#include "../meta_states/member.hpp"
#include "../meta_states/method.hpp"

#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_detail/value_utilities/uinst.hpp"

namespace meta_hpp
{
    template < typename... Args >
    uvalue invoke(const function& function, Args&&... args) {
        return function.invoke(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uresult try_invoke(const function& function, Args&&... args) {
        return function.try_invoke(META_HPP_FWD(args)...);
    }

    template < function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
    }

    template < function_pointer_kind Function, typename... Args >
    uresult try_invoke(Function function_ptr, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            // doesn't actually move 'args', just checks conversion errors
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
            if ( const uerror err = raw_function_invoke_error<Function>(registry, vargs) ) {
                return err;
            }
        }

        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
    }
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance) {
        return member.get(META_HPP_FWD(instance));
    }

    template < typename Instance >
    uresult try_invoke(const member& member, Instance&& instance) {
        return member.try_get(META_HPP_FWD(instance));
    }

    template < member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        return raw_member_getter<member_policy::as_copy_t>(registry, member_ptr, vinst);
    }

    template < member_pointer_kind Member, typename Instance >
    uresult try_invoke(Member member_ptr, Instance&& instance) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            // doesn't actually move an 'instance', just checks conversion errors
            const uinst_base vinst{registry, META_HPP_FWD(instance)};
            if ( const uerror err = raw_member_getter_error<Member>(registry, vinst) ) {
                return err;
            }
        }

        const uinst vinst{registry, META_HPP_FWD(instance)};
        return raw_member_getter<member_policy::as_copy_t>(registry, member_ptr, vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args >
    uresult try_invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.try_invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
    }

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            // doesn't actually move an 'instance' and 'args', just checks conversion errors
            const uinst_base vinst{registry, META_HPP_FWD(instance)};
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
            if ( const uerror err = raw_method_invoke_error<Method>(registry, vinst, vargs) ) {
                return err;
            }
        }

        const uinst vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
    }
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) noexcept {
        return !check_invocable_error<Args...>(function);
    }

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) noexcept {
        return !check_invocable_error(function, META_HPP_FWD(args)...);
    }

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr) noexcept {
        return !check_invocable_error<Args...>(function_ptr);
    }

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr, Args&&... args) noexcept {
        return !check_invocable_error(function_ptr, META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uerror check_invocable_error(const function& function) noexcept {
        return function.check_invocable_error<Args...>();
    }

    template < typename... Args >
    uerror check_invocable_error(const function& function, Args&&... args) noexcept {
        return function.check_invocable_error(META_HPP_FWD(args)...);
    }

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return raw_function_invoke_error<Function>(registry, vargs);
    }

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function, Args&&... args) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return raw_function_invoke_error<Function>(registry, vargs);
    }
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) noexcept {
        return !check_invocable_error<Instance>(member);
    }

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) noexcept {
        return !check_invocable_error(member, META_HPP_FWD(instance));
    }

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr) noexcept {
        return !check_invocable_error<Instance>(member_ptr);
    }

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr, Instance&& instance) noexcept {
        return !check_invocable_error(member_ptr, META_HPP_FWD(instance));
    }

    template < typename Instance >
    uerror check_invocable_error(const member& member) noexcept {
        return member.check_gettable_error<Instance>();
    }

    template < typename Instance >
    uerror check_invocable_error(const member& member, Instance&& instance) noexcept {
        return member.check_gettable_error(META_HPP_FWD(instance));
    }

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        return raw_member_getter_error<Member>(registry, vinst);
    }

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member, Instance&& instance) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        return raw_member_getter_error<Member>(registry, vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) noexcept {
        return !check_invocable_error<Instance, Args...>(method);
    }

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept {
        return !check_invocable_error(method, META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr) noexcept {
        return !check_invocable_error<Instance, Args...>(method_ptr);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr, Instance&& instance, Args&&... args) noexcept {
        return !check_invocable_error(method_ptr, META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method) noexcept {
        return method.check_invocable_error<Instance, Args...>();
    }

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method, Instance&& instance, Args&&... args) noexcept {
        return method.check_invocable_error(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return raw_method_invoke_error<Method>(registry, vinst, vargs);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method, Instance&& instance, Args&&... args) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return raw_method_invoke_error<Method>(registry, vinst, vargs);
    }
}
