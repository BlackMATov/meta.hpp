/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"
#include "../meta_utilities.hpp"

namespace meta_hpp
{
    template < typename... Args >
    std::optional<value> invoke(const function& function, Args&&... args) {
        return function.invoke(std::forward<Args>(args)...);
    }

    template < detail::function_kind Function, typename... Args >
    std::optional<value> invoke(Function&& function, Args&&... args) {
        using namespace detail;
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return vargs_invoke(std::forward<Function>(function), vargs);
        } else {
            return vargs_invoke(std::forward<Function>(function), {});
        }
    }
}

namespace meta_hpp
{
    template < typename Instance >
    std::optional<value> invoke(const member& member, Instance&& instance) {
        return member.get(std::forward<Instance>(instance));
    }

    template < detail::member_kind Member, typename Instance >
    std::optional<value> invoke(Member&& member, Instance&& instance) {
        using namespace detail;
        const inst vinst{std::forward<Instance>(instance)};
        return vargs_invoke(std::forward<Member>(member), vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    std::optional<value> invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < detail::method_kind Method, typename Instance, typename... Args >
    std::optional<value> invoke(Method&& method, Instance&& instance, Args&&... args) {
        using namespace detail;
        const inst vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return vargs_invoke(std::forward<Method>(method), vinst, vargs);
        } else {
            return vargs_invoke(std::forward<Method>(method), vinst, {});
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

    template < detail::function_kind Function, typename... Args >
    bool is_invocable_with() {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{type_list<Args>{}}...};
            return vargs_is_invocable_with<Function>(vargs);
        } else {
            return vargs_is_invocable_with<Function>({});
        }
    }

    template < detail::function_kind Function, typename... Args >
    bool is_invocable_with(Args&&... args) {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{std::forward<Args>(args)}...};
            return vargs_is_invocable_with<Function>(vargs);
        } else {
            return vargs_is_invocable_with<Function>({});
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
}

namespace meta_hpp
{
    template < detail::member_kind Member, typename Instance >
    bool is_invocable_with() {
        using namespace detail;
        const inst_base vinst{type_list<Instance>{}};
        return vargs_is_invocable_with<Member>(vinst);
    }

    template < detail::member_kind Member, typename Instance >
    bool is_invocable_with(Instance&& instance) {
        using namespace detail;
        const inst_base vinst{std::forward<Instance>(instance)};
        return vargs_is_invocable_with<Member>(vinst);
    }
}

namespace meta_hpp
{
    template < detail::method_kind Method, typename Instance, typename... Args >
    bool is_invocable_with() {
        using namespace detail;
        const inst_base vinst{type_list<Instance>{}};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{type_list<Args>{}}...};
            return vargs_is_invocable_with<Method>(vinst, vargs);
        } else {
            return vargs_is_invocable_with<Method>(vinst, {});
        }
    }

    template < detail::method_kind Method, typename Instance, typename... Args >
    bool is_invocable_with(Instance&& instance, Args&&... args) {
        using namespace detail;
        const inst_base vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{std::forward<Args>(args)}...};
            return vargs_is_invocable_with<Method>(vinst, vargs);
        } else {
            return vargs_is_invocable_with<Method>(vinst, {});
        }
    }
}
