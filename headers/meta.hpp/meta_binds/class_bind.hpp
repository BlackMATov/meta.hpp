/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"

#include "../meta_detail/type_registry.hpp"

namespace meta_hpp
{
    template < detail::class_kind Class >
    class_bind<Class>::class_bind(class_opts opts)
    : data_{detail::type_access(detail::resolve_type<Class>())} {
        data_->metadata.swap(opts.metadata);
        data_->metadata.merge(opts.metadata);
    }

    template < detail::class_kind Class >
    class_bind<Class>::operator class_type() const noexcept {
        return class_type{data_};
    }

    //
    // constructor_
    //

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
        constructor_opts opts,
        [[maybe_unused]] Policy policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        auto state = detail::constructor_state::make<Policy, Class, Args...>(std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match constructor argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        data_->constructors.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    //
    // destructor_
    //

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::destructor_()
        requires detail::class_bind_destructor_kind<Class>
    {
        return destructor_({});
    }

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::destructor_(destructor_opts opts)
        requires detail::class_bind_destructor_kind<Class>
    {
        auto state = detail::destructor_state::make<Class>(std::move(opts.metadata));
        data_->destructors.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    //
    // base_
    //

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

    //
    // function_
    //

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(
        std::string name,
        Function function,
        Policy policy)
    {
        return function_(std::move(name), std::move(function), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(
        std::string name,
        Function function,
        function_opts opts,
        [[maybe_unused]] Policy policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided arguments don't match function argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        data_->functions.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(
        std::string name,
        Function function,
        std::initializer_list<std::string_view> arguments,
        [[maybe_unused]] Policy policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            {});

        if ( arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match function argument count");
        }

        for ( std::size_t i = 0; i < arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        data_->functions.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    //
    // member_
    //

    template < detail::class_kind Class >
    template < detail::member_kind Member, member_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::member_(
        std::string name,
        Member member,
        Policy policy)
        requires detail::class_bind_member_kind<Class, Member>
    {
        return member_(std::move(name), std::move(member), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::member_kind Member, member_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::member_(
        std::string name,
        Member member,
        member_opts opts,
        [[maybe_unused]] Policy policy)
        requires detail::class_bind_member_kind<Class, Member>
    {
        auto state = detail::member_state::make<Policy>(
            std::move(name),
            std::move(member),
            std::move(opts.metadata));
        data_->members.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    //
    // method_
    //

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(
        std::string name,
        Method method,
        Policy policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        return method_(std::move(name), std::move(method), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(
        std::string name,
        Method method,
        method_opts opts,
        [[maybe_unused]] Policy policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        auto state = detail::method_state::make<Policy>(
            std::move(name),
            std::move(method),
            std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided arguments don't match method argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        data_->methods.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(
        std::string name,
        Method method,
        std::initializer_list<std::string_view> arguments,
        [[maybe_unused]] Policy policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        auto state = detail::method_state::make<Policy>(
            std::move(name),
            std::move(method),
            {});

        if ( arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match method argument count");
        }

        for ( std::size_t i = 0; i < arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        data_->methods.insert_or_assign(state->index, std::move(state));
        return *this;
    }

    //
    // variable_
    //

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::variable_(
        std::string name,
        Pointer pointer,
        Policy policy)
    {
        return variable_(std::move(name), std::move(pointer), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::variable_(
        std::string name,
        Pointer pointer,
        variable_opts opts,
        [[maybe_unused]] Policy policy)
    {
        auto state = detail::variable_state::make<Policy>(
            std::move(name),
            std::move(pointer),
            std::move(opts.metadata));
        data_->variables.insert_or_assign(state->index, std::move(state));
        return *this;
    }
}
