/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/ctor_type.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args, std::size_t... Is >
    value raw_ctor_invoke_impl(
        const arg* args,
        std::index_sequence<Is...>)
    {
        using ct = ctor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        if ( !(... && (args + Is)->can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
            throw std::logic_error("an attempt to call a constructor with an incorrect argument types");
        }

        class_type return_value{(args + Is)->cast<type_list_at_t<Is, argument_types>>()...};
        return value{std::forward<class_type>(return_value)};
    }

    template < class_kind Class, typename... Args >
    value raw_ctor_invoke(
        const arg* args,
        std::size_t arg_count)
    {
        using ct = ctor_traits<Class, Args...>;

        if ( arg_count != ct::arity ) {
            throw std::logic_error("an attempt to call a constructor with an incorrect arity");
        }

        return raw_ctor_invoke_impl<Class, Args...>(
            args,
            std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    ctor_state::invoke_impl make_ctor_invoke() {
        using namespace std::placeholders;
        return std::bind(&raw_ctor_invoke<Class, Args...>, _1, _2);
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args, std::size_t... Is >
    bool raw_ctor_is_invocable_with_impl(
        const arg_base* arg_bases,
        std::index_sequence<Is...>)
    {
        using ct = ctor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;
        return (... && (arg_bases + Is)->can_cast_to<type_list_at_t<Is, argument_types>>() );
    }

    template < class_kind Class, typename... Args >
    bool raw_ctor_is_invocable_with(
        const arg_base* arg_bases,
        std::size_t arg_count)
    {
        using ct = ctor_traits<Class, Args...>;

        if ( arg_count != ct::arity ) {
            return false;
        }

        return raw_ctor_is_invocable_with_impl<Class, Args...>(
            arg_bases,
            std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    ctor_state::is_invocable_with_impl make_ctor_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_ctor_is_invocable_with<Class, Args...>, _1, _2);
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    ctor_state_ptr ctor_state::make() {
        ctor_index index{ctor_type_data::get_static<Class, Args...>()};
        return std::make_shared<ctor_state>(std::move(index), type_list<Class>{}, type_list<Args...>{});
    }

    template < class_kind Class, typename... Args >
    ctor_state::ctor_state(ctor_index index, type_list<Class>, type_list<Args...>)
    : index{std::move(index)}
    , invoke{make_ctor_invoke<Class, Args...>()}
    , is_invocable_with{make_ctor_is_invocable_with<Class, Args...>()} {}
}

namespace meta_hpp
{
    inline ctor::ctor(detail::ctor_state_ptr state)
    : state_{std::move(state)} {}

    inline bool ctor::is_valid() const noexcept {
        return !!state_;
    }

    inline ctor::operator bool() const noexcept {
        return is_valid();
    }

    inline const ctor_index& ctor::get_index() const noexcept {
        return state_->index;
    }

    inline const ctor_type& ctor::get_type() const noexcept {
        return state_->index.type;
    }

    template < typename... Args >
    value ctor::invoke(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<detail::arg, sizeof...(Args)> vargs{detail::arg{std::forward<Args>(args)}...};
            return state_->invoke(vargs.data(), vargs.size());
        } else {
            return state_->invoke(nullptr, 0);
        }
    }

    template < typename... Args >
    value ctor::operator()(Args&&... args) const {
        return invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    bool ctor::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<detail::arg_base, sizeof...(Args)> arg_bases{detail::arg_base{detail::type_list<Args>{}}...};
            return state_->is_invocable_with(arg_bases.data(), arg_bases.size());
        } else {
            return state_->is_invocable_with(nullptr, 0);
        }
    }

    template < typename... Args >
    bool ctor::is_invocable_with(Args&&... args) const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<detail::arg, sizeof...(Args)> vargs{detail::arg{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs.data(), vargs.size());
        } else {
            return state_->is_invocable_with(nullptr, 0);
        }
    }
}
