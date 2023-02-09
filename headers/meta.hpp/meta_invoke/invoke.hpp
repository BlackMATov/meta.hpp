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

    template < detail::function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args) {
        using namespace detail;
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return raw_function_invoke<function_policy::as_copy_t>(function_ptr, vargs);
        } else {
            return raw_function_invoke<function_policy::as_copy_t>(function_ptr, {});
        }
    }
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance) {
        return member.get(std::forward<Instance>(instance));
    }

    template < detail::member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance) {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        return raw_member_getter<member_policy::as_copy_t>(member_ptr, vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args) {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return raw_method_invoke<method_policy::as_copy_t>(method_ptr, vinst, vargs);
        } else {
            return raw_method_invoke<method_policy::as_copy_t>(method_ptr, vinst, {});
        }
    }
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) {
        return function.is_invocable_with<Args...>();
    }

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) {
        return function.is_invocable_with(std::forward<Args>(args)...);
    }

    template < detail::function_pointer_kind Function, typename... Args >
    bool is_invocable_with() {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return raw_function_is_invocable_with<Function>(vargs);
        } else {
            return raw_function_is_invocable_with<Function>({});
        }
    }

    template < detail::function_pointer_kind Function, typename... Args >
    bool is_invocable_with(Args&&... args) {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return raw_function_is_invocable_with<Function>(vargs);
        } else {
            return raw_function_is_invocable_with<Function>({});
        }
    }
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) {
        return member.is_gettable_with<Instance>();
    }

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) {
        return member.is_gettable_with(std::forward<Instance>(instance));
    }

    template < detail::member_pointer_kind Member, typename Instance >
    bool is_invocable_with() {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        return raw_member_is_gettable_with<Member>(vinst);
    }

    template < detail::member_pointer_kind Member, typename Instance >
    bool is_invocable_with(Instance&& instance) {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        return raw_member_is_gettable_with<Member>(vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) {
        return method.is_invocable_with<Instance, Args...>();
    }

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) {
        return method.is_invocable_with(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    bool is_invocable_with() {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return raw_method_is_invocable_with<Method>(vinst, vargs);
        } else {
            return raw_method_is_invocable_with<Method>(vinst, {});
        }
    }

    template < detail::method_pointer_kind Method, typename Instance, typename... Args >
    bool is_invocable_with(Instance&& instance, Args&&... args) {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return raw_method_is_invocable_with<Method>(vinst, vargs);
        } else {
            return raw_method_is_invocable_with<Method>(vinst, {});
        }
    }
}
