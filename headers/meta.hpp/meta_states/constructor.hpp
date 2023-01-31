/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/constructor_type.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    uvalue raw_constructor_create(std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        constexpr bool as_object =
            std::is_copy_constructible_v<class_type> &&
            std::is_same_v<Policy, constructor_policy::as_object_t>;

        constexpr bool as_raw_ptr =
            std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>;

        constexpr bool as_shared_ptr =
            std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>;

        static_assert(as_object || as_raw_ptr || as_shared_ptr);

        if ( args.size() != ct::arity ) {
            META_HPP_THROW_AS(exception, "an attempt to call a constructor with an incorrect arity");
        }

        return [args]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                META_HPP_THROW_AS(exception, "an attempt to call a constructor with incorrect argument types");
            }

            if constexpr ( as_object ) {
                return make_uvalue<class_type>(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
            }

            if constexpr ( as_raw_ptr ) {
                return uvalue{std::make_unique<class_type>(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...).release()};
            }

            if constexpr ( as_shared_ptr ) {
                return uvalue{std::make_shared<class_type>(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
            }
        }(std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    uvalue raw_constructor_create_at(void* mem, std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        if ( args.size() != ct::arity ) {
            META_HPP_THROW_AS(exception, "an attempt to call a constructor with an incorrect arity");
        }

        return [mem, args]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                META_HPP_THROW_AS(exception, "an attempt to call a constructor with incorrect argument types");
            }
            return uvalue{std::construct_at(
                static_cast<class_type*>(mem),
                args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
        }(std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    bool raw_constructor_is_invocable_with(std::span<const uarg_base> args) {
        using ct = constructor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;

        if ( args.size() != ct::arity ) {
            return false;
        }

        return [args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>());
        }(std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    constructor_state::create_impl make_constructor_create() {
        return &raw_constructor_create<Policy, Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    constructor_state::create_at_impl make_constructor_create_at() {
        return &raw_constructor_create_at<Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    constructor_state::is_invocable_with_impl make_constructor_is_invocable_with() {
        return &raw_constructor_is_invocable_with<Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    argument_list make_constructor_arguments() {
        using ct = constructor_traits<Class, Args...>;
        using ct_argument_types = typename ct::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>){
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(std::index_sequence<I>){
                using P = type_list_at_t<I, ct_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(std::index_sequence<Is>{})...};
        }(std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    inline constructor_state::constructor_state(constructor_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    constructor_state_ptr constructor_state::make(metadata_map metadata) {
        constructor_state state{constructor_index::make<Class, Args...>(), std::move(metadata)};
        state.create = make_constructor_create<Policy, Class, Args...>();
        state.create_at = make_constructor_create_at<Class, Args...>();
        state.is_invocable_with = make_constructor_is_invocable_with<Class, Args...>();
        state.arguments = make_constructor_arguments<Class, Args...>();
        return make_intrusive<constructor_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline constructor::constructor(state_ptr state) noexcept
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

    inline const metadata_map& constructor::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const constructor_type& constructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename... Args >
    uvalue constructor::create(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->create(vargs);
        } else {
            return state_->create({});
        }
    }

    template < typename... Args >
    uvalue constructor::create_at(void* mem, Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->create_at(mem, vargs);
        } else {
            return state_->create_at(mem, {});
        }
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
