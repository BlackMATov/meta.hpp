/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/function_type.hpp"

namespace meta_hpp::detail
{
    template < function_kind Function, std::size_t... Is >
    std::optional<value> raw_function_invoke_impl(
        Function function,
        const arg* args,
        std::index_sequence<Is...>)
    {
        using ft = function_traits<Function>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        if ( !(... && (args + Is)->can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
            throw std::logic_error("an attempt to call a function with an incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(function,
                (args + Is)->cast<type_list_at_t<Is, argument_types>>()...);
            return std::nullopt;
        } else {
            return_type return_value{std::invoke(function,
                (args + Is)->cast<type_list_at_t<Is, argument_types>>()...)};
            return value{std::forward<return_type>(return_value)};
        }
    }

    template < function_kind Function >
    std::optional<value> raw_function_invoke(
        Function function,
        const arg* args,
        std::size_t arg_count)
    {
        using ft = function_traits<Function>;

        if ( arg_count != ft::arity ) {
            throw std::logic_error("an attempt to call a function with an incorrect arity");
        }

        return raw_function_invoke_impl<Function>(
            function,
            args,
            std::make_index_sequence<ft::arity>());
    }

    template < function_kind Function >
    function_state::invoke_impl make_function_invoke(Function function) {
        using namespace std::placeholders;
        return std::bind(&raw_function_invoke<Function>, function, _1, _2);
    }
}

namespace meta_hpp::detail
{
    template < function_kind Function, std::size_t... Is >
    bool raw_function_is_invocable_with_impl(
        const arg_base* args,
        std::index_sequence<Is...>)
    {
        using ft = function_traits<Function>;
        using argument_types = typename ft::argument_types;
        return (... && (args + Is)->can_cast_to<type_list_at_t<Is, argument_types>>() );
    }

    template < function_kind Function >
    bool raw_function_is_invocable_with(
        const arg_base* args,
        std::size_t arg_count)
    {
        using ft = function_traits<Function>;

        if ( arg_count != ft::arity ) {
            return false;
        }

        return raw_function_is_invocable_with_impl<Function>(
            args,
            std::make_index_sequence<ft::arity>());
    }

    template < function_kind Function >
    function_state::is_invocable_with_impl make_function_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_function_is_invocable_with<Function>, _1, _2);
    }
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    function_state_ptr function_state::make(std::string name, Function function) {
        function_index index{function_type_data::get_static<Function>(), std::move(name)};
        return std::make_shared<function_state>(std::move(index), std::move(function));
    }

    template < function_kind Function >
    function_state::function_state(function_index index, Function function)
    : index{std::move(index)}
    , invoke{make_function_invoke(function)}
    , is_invocable_with{make_function_is_invocable_with<Function>()} {}
}

namespace meta_hpp
{
    inline function::function(detail::function_state_ptr state)
    : state_{std::move(state)} {}

    inline bool function::is_valid() const noexcept {
        return !!state_;
    }

    inline function::operator bool() const noexcept {
        return is_valid();
    }

    inline const function_index& function::get_index() const noexcept {
        return state_->index;
    }

    inline const function_type& function::get_type() const noexcept {
        return state_->index.type;
    }

    inline const std::string& function::get_name() const noexcept {
        return state_->index.name;
    }

    template < typename... Args >
    std::optional<value> function::invoke(Args&&... args) const {
        using namespace detail;
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return state_->invoke(vargs.data(), vargs.size());
        } else {
            return state_->invoke(nullptr, 0);
        }
    }

    template < typename... Args >
    std::optional<value> function::operator()(Args&&... args) const {
        return invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    bool function::is_invocable_with() const noexcept {
        using namespace detail;
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<arg_base, sizeof...(Args)> vargs{arg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vargs.data(), vargs.size());
        } else {
            return state_->is_invocable_with(nullptr, 0);
        }
    }

    template < typename... Args >
    bool function::is_invocable_with(Args&&... args) const noexcept {
        using namespace detail;
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<arg_base, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs.data(), vargs.size());
        } else {
            return state_->is_invocable_with(nullptr, 0);
        }
    }
}
