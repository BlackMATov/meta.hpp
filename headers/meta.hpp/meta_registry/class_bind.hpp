/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    template < detail::class_kind Class >
    class_bind<Class>::class_bind()
    : data_{detail::get_type_data<Class>()} {}

    template < detail::class_kind Class >
    class_bind<Class>::operator class_type() const noexcept {
        return class_type{data_};
    }

    template < detail::class_kind Class >
    template < typename... Args >
    class_bind<Class>& class_bind<Class>::ctor_() {
        static_assert(std::is_constructible_v<Class, Args...>);
        auto ctor_state = detail::ctor_state::make<Class, Args...>();
        data_->ctors.emplace(ctor_state->index, std::move(ctor_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::class_kind Base >
    class_bind<Class>& class_bind<Class>::base_() {
        static_assert(std::is_base_of_v<Base, Class>);
        auto base_data = detail::class_type_data::get<Base>();
        data_->bases.emplace(base_data);
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::function_kind Function >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function) {
        auto function_state = detail::function_state::make<Function>(std::move(name), std::move(function));
        data_->functions.emplace(function_state->index, std::move(function_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::member_kind Member >
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member) {
        static_assert(std::same_as<Class, typename detail::member_traits<Member>::class_type>);
        auto member_state = detail::member_state::make<Member>(std::move(name), std::move(member));
        data_->members.emplace(member_state->index, std::move(member_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method >
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method) {
        static_assert(std::same_as<Class, typename detail::method_traits<Method>::class_type>);
        auto method_state = detail::method_state::make<Method>(std::move(name), std::move(method));
        data_->methods.emplace(method_state->index, std::move(method_state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer pointer) {
        auto variable_state = detail::variable_state::make<Pointer>(std::move(name), std::move(pointer));
        data_->variables.emplace(variable_state->index, std::move(variable_state));
        return *this;
    }
}
