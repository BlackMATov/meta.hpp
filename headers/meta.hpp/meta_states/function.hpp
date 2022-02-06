/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/function_type.hpp"
#include "../meta_detail/value_utilities/arg.hpp"

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    value raw_function_invoke(const Function& function, std::span<const arg> args) {
        using ft = function_traits<Function>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        constexpr bool as_copy =
            stdex::copy_constructible<return_type> &&
            stdex::same_as<Policy, function_policy::as_copy>;

        constexpr bool as_void =
            std::is_void_v<return_type> ||
            stdex::same_as<Policy, function_policy::discard_return>;

        constexpr bool ref_as_ptr =
            std::is_reference_v<return_type> &&
            stdex::same_as<Policy, function_policy::return_reference_as_pointer>;

        static_assert(as_copy || as_void || ref_as_ptr);

        if ( args.size() != ft::arity ) {
            throw std::logic_error("an attempt to call a function with an incorrect arity");
        }

        return std::invoke([
            &function, args
        // NOLINTNEXTLINE(readability-named-parameter)
        ]<std::size_t... Is>(std::index_sequence<Is...>) -> value {
            if ( !(... && (args.data() + Is)->can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw std::logic_error("an attempt to call a function with incorrect argument types");
            }

            if constexpr ( as_void ) {
                std::ignore = std::invoke(
                    function,
                    (args.data() + Is)->cast<type_list_at_t<Is, argument_types>>()...);
                return value{};
            } else {
                return_type&& return_value = std::invoke(
                    function,
                    (args.data() + Is)->cast<type_list_at_t<Is, argument_types>>()...);

                if constexpr ( ref_as_ptr ) {
                    return value{std::addressof(return_value)};
                } else {
                    return value{std::forward<decltype(return_value)>(return_value)};
                }
            }
        }, std::make_index_sequence<ft::arity>());
    }

    template < function_kind Function >
    bool raw_function_is_invocable_with(std::span<const arg_base> args) {
        using ft = function_traits<Function>;
        using argument_types = typename ft::argument_types;

        if ( args.size() != ft::arity ) {
            return false;
        }

        // NOLINTNEXTLINE(readability-named-parameter)
        return std::invoke([args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && (args.data() + Is)->can_cast_to<type_list_at_t<Is, argument_types>>());
        }, std::make_index_sequence<ft::arity>());
    }
}

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    function_state::invoke_impl make_function_invoke(Function function) {
        return [function = std::move(function)](std::span<const arg> args){
            return raw_function_invoke<Policy>(function, args);
        };
    }

    template < function_kind Function >
    function_state::is_invocable_with_impl make_function_is_invocable_with() {
        return &raw_function_is_invocable_with<Function>;
    }
}

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    function_state_ptr function_state::make(std::string name, Function function) {
        return std::make_shared<function_state>(function_state{
            .index{function_index::make<Function>(std::move(name))},
            .invoke{make_function_invoke<Policy>(std::move(function))},
            .is_invocable_with{make_function_is_invocable_with<Function>()},
        });
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
        return state_->index.get_type();
    }

    inline const std::string& function::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename... Args >
    value function::invoke(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return state_->invoke(vargs);
        } else {
            return state_->invoke({});
        }
    }

    template < typename... Args >
    value function::operator()(Args&&... args) const {
        return invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    bool function::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    template < typename... Args >
    bool function::is_invocable_with(Args&&... args) const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }
}
