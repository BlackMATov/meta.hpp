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
    template < typename... Args, constructor_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::constructor_(Policy policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        return constructor_<Args...>({}, policy);
    }

    template < detail::class_kind Class >
    template < typename... Args, constructor_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::constructor_(
        std::initializer_list<std::string_view> anames,
        [[maybe_unused]] Policy policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        auto constructor_state = detail::constructor_state::make<Policy, Class, Args...>();

        if ( anames.size() > constructor_state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match constructor argument count");
        }

        for ( std::size_t i = 0; i < anames.size(); ++i ) {
            argument& arg = constructor_state->arguments[i];
            detail::state_access(arg)->name = std::string{std::data(anames)[i]};
        }

        data_->constructors.emplace(constructor_state->index, std::move(constructor_state));
        return *this;
    }

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::destructor_()
        requires detail::class_bind_destructor_kind<Class>
    {
        auto destructor_state = detail::destructor_state::make<Class>();
        data_->destructors.emplace(destructor_state->index, std::move(destructor_state));
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
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function, Policy policy) {
        return function_(std::move(name), std::move(function), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(
        std::string name,
        Function function,
        std::initializer_list<std::string_view> anames,
        [[maybe_unused]] Policy policy)
    {
        auto function_state = detail::function_state::make<Policy>(std::move(name), std::move(function));

        if ( anames.size() > function_state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match function argument count");
        }

        for ( std::size_t i = 0; i < anames.size(); ++i ) {
            argument& arg = function_state->arguments[i];
            detail::state_access(arg)->name = std::string{std::data(anames)[i]};
        }

        data_->functions.emplace(function_state->index, std::move(function_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::member_kind Member, member_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member, [[maybe_unused]] Policy policy)
        requires detail::class_bind_member_kind<Class, Member>
    {
        auto member_state = detail::member_state::make<Policy>(std::move(name), std::move(member));
        data_->members.emplace(member_state->index, std::move(member_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method, Policy policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        return method_(std::move(name), std::move(method), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(
        std::string name,
        Method method,
        std::initializer_list<std::string_view> anames,
        [[maybe_unused]] Policy policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        auto method_state = detail::method_state::make<Policy>(std::move(name), std::move(method));

        if ( anames.size() > method_state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match method argument count");
        }

        for ( std::size_t i = 0; i < anames.size(); ++i ) {
            argument& arg = method_state->arguments[i];
            detail::state_access(arg)->name = std::string{std::data(anames)[i]};
        }

        data_->methods.emplace(method_state->index, std::move(method_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::variable_( std::string name, Pointer pointer, [[maybe_unused]] Policy policy) {
        auto variable_state = detail::variable_state::make<Policy>(std::move(name), std::move(pointer));
        data_->variables.emplace(variable_state->index, std::move(variable_state));
        return *this;
    }
}
