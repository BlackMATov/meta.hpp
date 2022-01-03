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
    template < function_kind Function >
    std::optional<value> raw_function_invoke(Function function, std::span<arg> args) {
        using ft = function_traits<Function>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        if ( args.size() != ft::arity ) {
            throw std::logic_error("an attempt to call a function with an incorrect arity");
        }

        // NOLINTNEXTLINE(readability-named-parameter)
        return std::invoke([&function, &args]<std::size_t... Is>(std::index_sequence<Is...>){
            if ( !(... && (args.data() + Is)->can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw std::logic_error("an attempt to call a function with incorrect argument types");
            }

            if constexpr ( std::is_void_v<return_type> ) {
                std::invoke(
                    std::move(function),
                    (args.data() + Is)->cast<type_list_at_t<Is, argument_types>>()...);
                return std::nullopt;
            } else {
                return_type return_value{std::invoke(
                    std::move(function),
                    (args.data() + Is)->cast<type_list_at_t<Is, argument_types>>()...)};
                return value{std::forward<return_type>(return_value)};
            }
        }, std::make_index_sequence<ft::arity>());
    }

    template < function_kind Function >
    function_state::invoke_impl make_function_invoke(Function function) {
        using namespace std::placeholders;
        return std::bind(&raw_function_invoke<Function>, function, _1);
    }
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    bool raw_function_is_invocable_with(std::span<arg_base> args) {
        using ft = function_traits<Function>;
        using argument_types = typename ft::argument_types;

        if ( args.size() != ft::arity ) {
            return false;
        }

        // NOLINTNEXTLINE(readability-named-parameter)
        return std::invoke([&args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && (args.data() + Is)->can_cast_to<type_list_at_t<Is, argument_types>>());
        }, std::make_index_sequence<ft::arity>());
    }

    template < function_kind Function >
    function_state::is_invocable_with_impl make_function_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_function_is_invocable_with<Function>, _1);
    }
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    function_state::function_state(function_index index, Function function)
    : index{std::move(index)}
    , invoke{make_function_invoke(function)}
    , is_invocable_with{make_function_is_invocable_with<Function>()} {}

    template < function_kind Function >
    function_state_ptr function_state::make(std::string name, Function function) {
        function_index index{function_type_data::get_static<Function>(), std::move(name)};
        return std::make_shared<function_state>(std::move(index), std::move(function));
    }
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
            return state_->invoke(vargs);
        } else {
            return state_->invoke({});
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
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    template < typename... Args >
    bool function::is_invocable_with(Args&&... args) const noexcept {
        using namespace detail;
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<arg_base, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }
}
