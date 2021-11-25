/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/method_type.hpp"

namespace meta_hpp::detail
{
    template < method_kind Method, std::size_t... Is >
    std::optional<value> raw_method_invoke_impl(
        Method method,
        const inst& inst,
        const arg* args,
        std::index_sequence<Is...>)
    {
        using mt = method_traits<Method>;
        using return_type = typename mt::return_type;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( !inst.can_cast_to<qualified_type>() ) {
            throw std::logic_error("an attempt to call a method with an incorrect instance type");
        }

        if ( !(... && (args + Is)->can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
            throw std::logic_error("an attempt to call a method with an incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(method,
                inst.cast<qualified_type>(),
                (args + Is)->cast<type_list_at_t<Is, argument_types>>()...);
            return std::nullopt;
        } else {
            return_type return_value{std::invoke(method,
                inst.cast<qualified_type>(),
                (args + Is)->cast<type_list_at_t<Is, argument_types>>()...)};
            return value{std::forward<return_type>(return_value)};
        }
    }

    template < method_kind Method >
    std::optional<value> raw_method_invoke(
        Method method,
        const inst& inst,
        const arg* args,
        std::size_t arg_count)
    {
        using mt = method_traits<Method>;

        if ( arg_count != mt::arity ) {
            throw std::logic_error("an attempt to call a method with an incorrect arity");
        }

        return raw_method_invoke_impl<Method>(
            method,
            inst,
            args,
            std::make_index_sequence<mt::arity>());
    }

    template < method_kind Method >
    method_state::invoke_impl make_method_invoke(Method method) {
        using namespace std::placeholders;
        return std::bind(&raw_method_invoke<Method>, method, _1, _2, _3);
    }
}

namespace meta_hpp::detail
{
    template < method_kind Method, std::size_t... Is >
    bool raw_method_is_invocable_with_impl(
        const inst_base& inst_base,
        const arg_base* arg_bases,
        std::index_sequence<Is...>)
    {
        using mt = method_traits<Method>;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        return inst_base.can_cast_to<qualified_type>()
            && (... && (arg_bases + Is)->can_cast_to<type_list_at_t<Is, argument_types>>() );
    }

    template < method_kind Method >
    bool raw_method_is_invocable_with(
        const inst_base& inst_base,
        const arg_base* arg_bases,
        std::size_t arg_count)
    {
        using mt = method_traits<Method>;

        if ( arg_count != mt::arity ) {
            return false;
        }

        return raw_method_is_invocable_with_impl<Method>(
            inst_base,
            arg_bases,
            std::make_index_sequence<mt::arity>());
    }

    template < method_kind Method >
    method_state::is_invocable_with_impl make_method_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_method_is_invocable_with<Method>, _1, _2, _3);
    }
}

namespace meta_hpp::detail
{
    template < method_kind Method >
    method_state_ptr method_state::make(std::string name, Method method) {
        method_index index{method_type_data::get<Method>(), std::move(name)};
        return std::make_shared<method_state>(std::move(index), std::move(method));
    }

    template < method_kind Method >
    method_state::method_state(method_index index, Method method)
    : index{std::move(index)}
    , invoke{make_method_invoke(method)}
    , is_invocable_with{make_method_is_invocable_with<Method>()} {}
}

namespace meta_hpp
{
    inline method::method(detail::method_state_ptr state)
    : state_{std::move(state)} {}

    inline bool method::is_valid() const noexcept {
        return !!state_;
    }

    inline method::operator bool() const noexcept {
        return is_valid();
    }

    inline const method_index& method::get_index() const noexcept {
        return state_->index;
    }

    inline const method_type& method::get_type() const noexcept {
        return state_->index.type;
    }

    inline const std::string& method::get_name() const noexcept {
        return state_->index.name;
    }

    template < typename Instance, typename... Args >
    std::optional<value> method::invoke(Instance&& instance, Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<detail::arg, sizeof...(Args)> vargs{detail::arg{std::forward<Args>(args)}...};
            return state_->invoke(detail::inst{std::forward<Instance>(instance)}, vargs.data(), vargs.size());
        } else {
            return state_->invoke(detail::inst{std::forward<Instance>(instance)}, nullptr, 0);
        }
    }

    template < typename Inst, typename... Args >
    bool method::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<detail::arg_base, sizeof...(Args)> arg_bases{detail::arg_base{detail::type_list<Args>{}}...};
            return state_->is_invocable_with(detail::inst_base{detail::type_list<Inst>{}}, arg_bases.data(), arg_bases.size());
        } else {
            return state_->is_invocable_with(detail::inst_base{detail::type_list<Inst>{}}, nullptr, 0);
        }
    }
}
