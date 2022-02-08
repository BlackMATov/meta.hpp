/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/constructor_type.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    uvalue raw_constructor_invoke(std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        constexpr bool as_object =
            stdex::copy_constructible<class_type> &&
            stdex::same_as<Policy, constructor_policy::as_object>;

        constexpr bool as_raw_ptr =
            stdex::same_as<Policy, constructor_policy::as_raw_pointer>;

        constexpr bool as_shared_ptr =
            stdex::same_as<Policy, constructor_policy::as_shared_pointer>;

        static_assert(as_object || as_raw_ptr || as_shared_ptr);

        if ( args.size() != ct::arity ) {
            throw_exception_with("an attempt to call a constructor with an incorrect arity");
        }

        return std::invoke([
            args
        // NOLINTNEXTLINE(readability-named-parameter)
        ]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw_exception_with("an attempt to call a constructor with incorrect argument types");
            }

            if constexpr ( as_object ) {
                class_type return_value{args[Is].cast<type_list_at_t<Is, argument_types>>()...};
                return uvalue{std::move(return_value)};
            }

            if constexpr ( as_raw_ptr ) {
                auto return_value{std::make_unique<class_type>(args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
                return uvalue{return_value.release()};
            }

            if constexpr ( as_shared_ptr ) {
                auto return_value{std::make_shared<class_type>(args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
                return uvalue{std::move(return_value)};
            }
        }, std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    bool raw_constructor_is_invocable_with(std::span<const uarg_base> args) {
        using ct = constructor_traits<Class, Args...>;
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
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    constructor_state::invoke_impl make_constructor_invoke() {
        return &raw_constructor_invoke<Policy, Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    constructor_state::is_invocable_with_impl make_constructor_is_invocable_with() {
        return &raw_constructor_is_invocable_with<Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    argument_list make_constructor_arguments() {
        using ct = detail::constructor_traits<Class, Args...>;

        argument_list arguments;
        arguments.reserve(ct::arity);

        // NOLINTNEXTLINE(readability-named-parameter)
        [&arguments]<std::size_t... Is>(std::index_sequence<Is...>) mutable {
            (arguments.push_back([]<std::size_t I>(){
                using P = detail::type_list_at_t<I, typename ct::argument_types>;
                return argument{detail::argument_state::make<P>(I)};
            }.template operator()<Is>()), ...);
        }(std::make_index_sequence<ct::arity>());

        return arguments;
    }
}

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    constructor_state_ptr constructor_state::make() {
        return std::make_shared<constructor_state>(constructor_state{
            .index{constructor_index::make<Class, Args...>()},
            .invoke{make_constructor_invoke<Policy, Class, Args...>()},
            .is_invocable_with{make_constructor_is_invocable_with<Class, Args...>()},
            .arguments{make_constructor_arguments<Class, Args...>()},
        });
    }
}

namespace meta_hpp
{
    inline constructor::constructor(detail::constructor_state_ptr state)
    : state_{std::move(state)} {}

    inline bool constructor::is_valid() const noexcept {
        return !!state_;
    }

    inline constructor::operator bool() const noexcept {
        return is_valid();
    }

    inline const constructor_index& constructor::get_index() const noexcept {
        return state_->index;
    }

    inline const constructor_type& constructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename... Args >
    uvalue constructor::invoke(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->invoke(vargs);
        } else {
            return state_->invoke({});
        }
    }

    template < typename... Args >
    uvalue constructor::operator()(Args&&... args) const {
        return invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    bool constructor::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    template < typename... Args >
    bool constructor::is_invocable_with(Args&&... args) const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    inline argument constructor::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& constructor::get_arguments() const noexcept {
        return state_->arguments;
    }
}
