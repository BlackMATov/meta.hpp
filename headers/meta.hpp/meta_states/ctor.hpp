/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/ctor_type.hpp"
#include "../meta_detail/value_utilities/arg.hpp"

namespace meta_hpp::detail
{
    template < ctor_policy_kind Policy, class_kind Class, typename... Args >
    value raw_ctor_invoke(std::span<const arg> args) {
        using ct = ctor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        constexpr bool as_object =
            stdex::copy_constructible<class_type> &&
            stdex::same_as<Policy, ctor_policy::as_object>;

        constexpr bool as_raw_ptr =
            stdex::same_as<Policy, ctor_policy::as_raw_pointer>;

        constexpr bool as_shared_ptr =
            stdex::same_as<Policy, ctor_policy::as_shared_pointer>;

        static_assert(as_object || as_raw_ptr || as_shared_ptr);

        if ( args.size() != ct::arity ) {
            throw_exception_with("an attempt to call a constructor with an incorrect arity");
        }

        return std::invoke([
            args
        // NOLINTNEXTLINE(readability-named-parameter)
        ]<std::size_t... Is>(std::index_sequence<Is...>) -> value {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw_exception_with("an attempt to call a constructor with incorrect argument types");
            }

            if constexpr ( as_object ) {
                class_type return_value{args[Is].cast<type_list_at_t<Is, argument_types>>()...};
                return value{std::move(return_value)};
            }

            if constexpr ( as_raw_ptr ) {
                auto return_value{std::make_unique<class_type>(args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
                return value{return_value.release()};
            }

            if constexpr ( as_shared_ptr ) {
                auto return_value{std::make_shared<class_type>(args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
                return value{std::move(return_value)};
            }
        }, std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    bool raw_ctor_is_invocable_with(std::span<const arg_base> args) {
        using ct = ctor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;

        if ( args.size() != ct::arity ) {
            return false;
        }

        // NOLINTNEXTLINE(readability-named-parameter)
        return std::invoke([args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>());
        }, std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    template < ctor_policy_kind Policy, class_kind Class, typename... Args >
    ctor_state::invoke_impl make_ctor_invoke() {
        return &raw_ctor_invoke<Policy, Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    ctor_state::is_invocable_with_impl make_ctor_is_invocable_with() {
        return &raw_ctor_is_invocable_with<Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    parameter_list make_ctor_parameters() {
        using ct = detail::ctor_traits<Class, Args...>;

        parameter_list parameters;
        parameters.reserve(ct::arity);

        // NOLINTNEXTLINE(readability-named-parameter)
        [&parameters]<std::size_t... Is>(std::index_sequence<Is...>) mutable {
            (parameters.push_back([]<std::size_t I>(){
                using P = detail::type_list_at_t<I, typename ct::argument_types>;
                return parameter{detail::parameter_state::make<P>(I)};
            }.template operator()<Is>()), ...);
        }(std::make_index_sequence<ct::arity>());

        return parameters;
    }
}

namespace meta_hpp::detail
{
    template < ctor_policy_kind Policy, class_kind Class, typename... Args >
    ctor_state_ptr ctor_state::make() {
        return std::make_shared<ctor_state>(ctor_state{
            .index{ctor_index::make<Class, Args...>()},
            .invoke{make_ctor_invoke<Policy, Class, Args...>()},
            .is_invocable_with{make_ctor_is_invocable_with<Class, Args...>()},
            .parameters{make_ctor_parameters<Class, Args...>()},
        });
    }
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
        return state_->index.get_type();
    }

    template < typename... Args >
    value ctor::invoke(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return state_->invoke(vargs);
        } else {
            return state_->invoke({});
        }
    }

    template < typename... Args >
    value ctor::operator()(Args&&... args) const {
        return invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    bool ctor::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    template < typename... Args >
    bool ctor::is_invocable_with(Args&&... args) const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    inline parameter ctor::get_parameter(std::size_t position) const noexcept {
        return position < state_->parameters.size() ? state_->parameters[position] : parameter{};
    }

    inline const parameter_list& ctor::get_parameters() const noexcept {
        return state_->parameters;
    }
}
