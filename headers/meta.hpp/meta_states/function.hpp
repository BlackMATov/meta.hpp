/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/function_type.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    uvalue raw_function_invoke(const Function& function, std::span<const uarg> args) {
        using ft = function_traits<Function>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        constexpr bool as_copy =
            std::is_copy_constructible_v<return_type> &&
            std::is_same_v<Policy, function_policy::as_copy_t>;

        constexpr bool as_void =
            std::is_void_v<return_type> ||
            std::is_same_v<Policy, function_policy::discard_return_t>;

        constexpr bool ref_as_ptr =
            std::is_reference_v<return_type> &&
            std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>;

        static_assert(as_copy || as_void || ref_as_ptr);

        if ( args.size() != ft::arity ) {
            META_HPP_THROW_AS(exception, "an attempt to call a function with an incorrect arity");
        }

        return [&function, args]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                META_HPP_THROW_AS(exception, "an attempt to call a function with incorrect argument types");
            }

            if constexpr ( std::is_void_v<return_type> ) {
                function(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
                return uvalue{};
            } else if constexpr ( std::is_same_v<Policy, function_policy::discard_return_t> ) {
                std::ignore = function(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
                return uvalue{};
            } else {
                return_type&& return_value = function(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);

                if constexpr ( ref_as_ptr ) {
                    return uvalue{std::addressof(return_value)};
                } else {
                    return uvalue{std::forward<decltype(return_value)>(return_value)};
                }
            }
        }(std::make_index_sequence<ft::arity>());
    }

    template < function_kind Function >
    bool raw_function_is_invocable_with(std::span<const uarg_base> args) {
        using ft = function_traits<Function>;
        using argument_types = typename ft::argument_types;

        if ( args.size() != ft::arity ) {
            return false;
        }

        return [args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>());
        }(std::make_index_sequence<ft::arity>());
    }
}

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    function_state::invoke_impl make_function_invoke(Function function) {
        return [function = std::move(function)](std::span<const uarg> args){
            return raw_function_invoke<Policy>(function, args);
        };
    }

    template < function_kind Function >
    function_state::is_invocable_with_impl make_function_is_invocable_with() {
        return &raw_function_is_invocable_with<Function>;
    }

    template < function_kind Function >
    argument_list make_function_arguments() {
        using ft = function_traits<Function>;
        using ft_argument_types = typename ft::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>){
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(std::index_sequence<I>){
                using P = type_list_at_t<I, ft_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(std::index_sequence<Is>{})...};
        }(std::make_index_sequence<ft::arity>());
    }
}

namespace meta_hpp::detail
{
    inline function_state::function_state(function_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < function_policy_kind Policy, function_kind Function >
    function_state_ptr function_state::make(std::string name, Function function, metadata_map metadata) {
        function_state state{function_index::make<Function>(std::move(name)), std::move(metadata)};
        state.invoke = make_function_invoke<Policy>(std::move(function));
        state.is_invocable_with = make_function_is_invocable_with<Function>();
        state.arguments = make_function_arguments<Function>();
        return make_intrusive<function_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline function::function(state_ptr state) noexcept
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

    inline const metadata_map& function::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const function_type& function::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& function::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename... Args >
    uvalue function::invoke(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->invoke(vargs);
        } else {
            return state_->invoke({});
        }
    }

    template < typename... Args >
    uvalue function::operator()(Args&&... args) const {
        return invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    bool function::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    template < typename... Args >
    bool function::is_invocable_with(Args&&... args) const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    inline argument function::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& function::get_arguments() const noexcept {
        return state_->arguments;
    }
}
