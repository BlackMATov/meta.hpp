/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_invoke.hpp"
#include "../meta_states.hpp"

#include "../meta_states/function.hpp"
#include "../meta_states/member.hpp"
#include "../meta_states/method.hpp"

#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_detail/value_utilities/uinst.hpp"

namespace meta_hpp
{
    template < typename... Args >
    uvalue invoke(const function& function, Args&&... args) {
        return function.invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    uresult try_invoke(const function& function, Args&&... args) {
        return function.try_invoke(std::forward<Args>(args)...);
    }

    template < detail::function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, std::forward<Args>(args)}...};
        return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
    }

    template < detail::function_pointer_kind Function, typename... Args >
    uresult try_invoke(Function function_ptr, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, std::forward<Args>(args)}...};
            if ( const uerror err = raw_function_invoke_error<Function>(registry, vargs) ) {
                return err;
            }
        }

        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, std::forward<Args>(args)}...};
        return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
    }
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance) {
        return member.get(std::forward<Instance>(instance));
    }

    template < typename Instance >
    uresult try_invoke(const member& member, Instance&& instance) {
        return member.try_get(std::forward<Instance>(instance));
    }

    template < detail::member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, std::forward<Instance>(instance)};
        return raw_member_getter<member_policy::as_copy_t>(registry, member_ptr, vinst);
    }

    template < detail::member_pointer_kind Member, typename Instance >
    uresult try_invoke(Member member_ptr, Instance&& instance) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            const uinst_base vinst{registry, std::forward<Instance>(instance)};
            if ( const uerror err = raw_member_getter_error<Member>(registry, vinst) ) {
                return err;
            }
        }

        const uinst vinst{registry, std::forward<Instance>(instance)};
        return raw_member_getter<member_policy::as_copy_t>(registry, member_ptr, vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < typename Instance, typename... Args >
    uresult try_invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.try_invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, std::forward<Instance>(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, std::forward<Args>(args)}...};
        return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
    }

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            const uinst_base vinst{registry, std::forward<Instance>(instance)};
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, std::forward<Args>(args)}...};
            if ( const uerror err = raw_method_invoke_error<Method>(registry, vinst, vargs) ) {
                return err;
            }
        }

        const uinst vinst{registry, std::forward<Instance>(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, std::forward<Args>(args)}...};
        return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
    }
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) noexcept {
        return function.is_invocable_with<Args...>();
    }

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) noexcept {
        return function.is_invocable_with(std::forward<Args>(args)...);
    }

    template < typename... Args, detail::function_pointer_kind Function >
    bool is_invocable_with(Function) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return !raw_function_invoke_error<Function>(registry, vargs);
    }

    template < typename... Args, detail::function_pointer_kind Function >
    bool is_invocable_with(Function, Args&&... args) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, std::forward<Args>(args)}...};
        return !raw_function_invoke_error<Function>(registry, vargs);
    }
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) noexcept {
        return member.is_gettable_with<Instance>();
    }

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) noexcept {
        return member.is_gettable_with(std::forward<Instance>(instance));
    }

    template < typename Instance, detail::member_pointer_kind Member >
    bool is_invocable_with(Member) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        return !raw_member_getter_error<Member>(registry, vinst);
    }

    template < typename Instance, detail::member_pointer_kind Member >
    bool is_invocable_with(Member, Instance&& instance) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, std::forward<Instance>(instance)};
        return !raw_member_getter_error<Member>(registry, vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) noexcept {
        return method.is_invocable_with<Instance, Args...>();
    }

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept {
        return method.is_invocable_with(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < typename Instance, typename... Args, detail::method_pointer_kind Method >
    bool is_invocable_with(Method) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return !raw_method_invoke_error<Method>(registry, vinst, vargs);
    }

    template < typename Instance, typename... Args, detail::method_pointer_kind Method >
    bool is_invocable_with(Method, Instance&& instance, Args&&... args) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, std::forward<Instance>(instance)};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, std::forward<Args>(args)}...};
        return !raw_method_invoke_error<Method>(registry, vinst, vargs);
    }
}
