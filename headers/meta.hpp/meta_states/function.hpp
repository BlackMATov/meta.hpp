/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_types/function_type.hpp"

namespace meta_hpp::detail
{
    template < function_policy_family Policy, function_pointer_kind Function >
    uvalue raw_function_invoke(type_registry& registry, Function function_ptr, std::span<const uarg> args) {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        constexpr bool as_copy                             //
            = std::is_constructible_v<uvalue, return_type> //
           && std::is_same_v<Policy, function_policy::as_copy_t>;

        constexpr bool as_void            //
            = std::is_void_v<return_type> //
           || std::is_same_v<Policy, function_policy::discard_return_t>;

        constexpr bool ref_as_ptr              //
            = std::is_reference_v<return_type> //
           && std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>;

        static_assert(as_copy || as_void || ref_as_ptr);

        META_HPP_ASSERT(             //
            args.size() == ft::arity //
            && "an attempt to call a function with an incorrect arity"
        );

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a function with incorrect argument types"
        );

        return unchecked_call_with_uargs<argument_types>(registry, args, [function_ptr](auto&&... all_args) {
            if constexpr ( std::is_void_v<return_type> ) {
                function_ptr(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( std::is_same_v<Policy, function_policy::discard_return_t> ) {
                std::ignore = function_ptr(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( !std::is_void_v<return_type> ) {
                return_type&& result = function_ptr(META_HPP_FWD(all_args)...);
                return ref_as_ptr ? uvalue{std::addressof(result)} : uvalue{META_HPP_FWD(result)};
            }
        });
    }

    template < function_pointer_kind Function >
    uerror raw_function_invoke_error(type_registry& registry, std::span<const uarg_base> args) noexcept {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        using argument_types = typename ft::argument_types;

        if ( args.size() != ft::arity ) {
            return uerror{error_code::arity_mismatch};
        }

        if ( !can_cast_all_uargs<argument_types>(registry, args) ) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < function_policy_family Policy, function_pointer_kind Function >
    function_state::invoke_impl make_function_invoke(type_registry& registry, Function function_ptr) {
        return [&registry, function_ptr](std::span<const uarg> args) { //
            return raw_function_invoke<Policy>(registry, function_ptr, args);
        };
    }

    template < function_pointer_kind Function >
    function_state::invoke_error_impl make_function_invoke_error(type_registry& registry) {
        return [&registry](std::span<const uarg_base> args) { //
            return raw_function_invoke_error<Function>(registry, args);
        };
    }

    template < function_pointer_kind Function >
    argument_list make_function_arguments() {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        using ft_argument_types = typename ft::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
                using P = type_list_at_t<I, ft_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(index_constant<Is>{})...};
        }(std::make_index_sequence<ft::arity>());
    }
}

namespace meta_hpp::detail
{
    inline function_state::function_state(function_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < function_policy_family Policy, function_pointer_kind Function >
    function_state::state_ptr function_state::make(std::string name, Function function_ptr, metadata_map metadata) {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        type_registry& registry{type_registry::instance()};

        function_state state{
            function_index{registry.resolve_by_traits<ft>(), std::move(name)},
            std::move(metadata),
        };

        state.invoke = make_function_invoke<Policy>(registry, function_ptr);
        state.invoke_error = make_function_invoke_error<Function>(registry);
        state.arguments = make_function_arguments<Function>();

        return std::make_shared<function_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline function_type function::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& function::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline std::size_t function::get_arity() const noexcept {
        return state_->arguments.size();
    }

    inline argument function::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& function::get_arguments() const noexcept {
        return state_->arguments;
    }

    template < typename... Args >
    uvalue function::invoke(Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->invoke(vargs);
    }

    template < typename... Args >
    uresult function::try_invoke(Args&&... args) const {
        // doesn't actually move 'args', just checks conversion errors
        if ( const uerror err = check_invocable_error(META_HPP_FWD(args)...) ) {
            return err;
        }
        return invoke(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uvalue function::operator()(Args&&... args) const {
        return invoke(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    bool function::is_invocable_with() const {
        return !check_invocable_error<Args...>();
    }

    template < typename... Args >
    bool function::is_invocable_with(Args&&... args) const {
        return !check_invocable_error(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uerror function::check_invocable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return state_->invoke_error(vargs);
    }

    template < typename... Args >
    uerror function::check_invocable_error(Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return state_->invoke_error(vargs);
    }

    template < typename Iter >
    uvalue function::invoke_variadic(Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg) * arity);
        detail::inline_vector<uarg> vargs{static_cast<uarg*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                throw_exception(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        return state_->invoke({vargs.begin(), vargs.end()});
    }

    template < typename Iter >
    uresult function::try_invoke_variadic(Iter first, Iter last) const {
        if ( const uerror err = check_variadic_invocable_error(first, last) ) {
            return err;
        }
        return invoke_variadic(first, last);
    }

    template < typename Iter >
    bool function::is_variadic_invocable_with(Iter first, Iter last) const {
        return !check_variadic_invocable_error(first, last);
    }

    template < typename Iter >
    uerror function::check_variadic_invocable_error(Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg_base) * arity);
        detail::inline_vector<uarg_base> vargs{static_cast<uarg_base*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                return uerror(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        return state_->invoke_error({vargs.begin(), vargs.end()});
    }
}
