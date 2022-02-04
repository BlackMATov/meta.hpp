/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"

#include "../meta_detail/type_registry.hpp"

namespace meta_hpp
{
    template < detail::class_kind Class >
    class_bind<Class>::class_bind()
    : data_{detail::type_access(detail::resolve_type<Class>())} {}

    template < detail::class_kind Class >
    class_bind<Class>::operator class_type() const noexcept {
        return class_type{data_};
    }

    template < detail::class_kind Class >
    template < typename... Args, ctor_policy_kind Policy >
    // NOLINTNEXTLINE(readability-named-parameter)
    class_bind<Class>& class_bind<Class>::ctor_(Policy)
        requires detail::class_bind_ctor_kind<Class, Args...>
    {
        auto ctor_state = detail::ctor_state::make<Policy, Class, Args...>();
        data_->ctors.emplace(ctor_state->index, std::move(ctor_state));
        return *this;
    }

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::dtor_()
        requires detail::class_bind_dtor_kind<Class>
    {
        auto dtor_state = detail::dtor_state::make<Class>();
        data_->dtors.emplace(dtor_state->index, std::move(dtor_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::class_kind Base >
    class_bind<Class>& class_bind<Class>::base_()
        requires detail::class_bind_base_kind<Class, Base>
    {
        data_->bases.emplace(detail::resolve_type<Base>());
        data_->bases_info.emplace(detail::resolve_type<Base>(), detail::class_type_data::base_info{
            .upcast = +[](void* derived) -> void* {
                return static_cast<Base*>(static_cast<Class*>(derived));
            }
        });
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    // NOLINTNEXTLINE(readability-named-parameter)
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function, Policy) {
        auto function_state = detail::function_state::make<Policy>(std::move(name), std::move(function));
        data_->functions.emplace(function_state->index, std::move(function_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::member_kind Member, member_policy_kind Policy >
    // NOLINTNEXTLINE(readability-named-parameter)
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member, Policy)
        requires detail::class_bind_member_kind<Class, Member>
    {
        auto member_state = detail::member_state::make<Policy>(std::move(name), std::move(member));
        data_->members.emplace(member_state->index, std::move(member_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    // NOLINTNEXTLINE(readability-named-parameter)
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method, Policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        auto method_state = detail::method_state::make<Policy>(std::move(name), std::move(method));
        data_->methods.emplace(method_state->index, std::move(method_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    // NOLINTNEXTLINE(readability-named-parameter)
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer pointer, Policy) {
        auto variable_state = detail::variable_state::make<Policy>(std::move(name), std::move(pointer));
        data_->variables.emplace(variable_state->index, std::move(variable_state));
        return *this;
    }
}
