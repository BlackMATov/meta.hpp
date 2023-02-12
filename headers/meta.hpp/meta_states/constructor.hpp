/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_types/constructor_type.hpp"

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    uvalue raw_constructor_create(std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        constexpr bool as_object                       //
            = std::is_copy_constructible_v<class_type> //
           && std::is_same_v<Policy, constructor_policy::as_object_t>;

        constexpr bool as_raw_ptr //
            = std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>;

        constexpr bool as_shared_ptr //
            = std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>;

        static_assert(as_object || as_raw_ptr || as_shared_ptr);

        META_HPP_ASSERT(             //
            args.size() == ct::arity //
            && "an attempt to call a constructor with an incorrect arity"
        );

        META_HPP_ASSERT(                             //
            can_cast_all_uargs<argument_types>(args) //
            && "an attempt to call a constructor with incorrect argument types"
        );

        return call_with_uargs<argument_types>(args, [](auto&&... all_args) -> uvalue {
            if constexpr ( as_object ) {
                return make_uvalue<class_type>(META_HPP_FWD(all_args)...);
            }

            if constexpr ( as_raw_ptr ) {
                return std::make_unique<class_type>(META_HPP_FWD(all_args)...).release();
            }

            if constexpr ( as_shared_ptr ) {
                return std::make_shared<class_type>(META_HPP_FWD(all_args)...);
            }
        });
    }

    template < class_kind Class, typename... Args >
    uvalue raw_constructor_create_at(void* mem, std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        META_HPP_ASSERT(             //
            args.size() == ct::arity //
            && "an attempt to call a constructor with an incorrect arity"
        );

        META_HPP_ASSERT(                             //
            can_cast_all_uargs<argument_types>(args) //
            && "an attempt to call a constructor with incorrect argument types"
        );

        return call_with_uargs<argument_types>(args, [mem](auto&&... all_args) {
            return std::construct_at(static_cast<class_type*>(mem), META_HPP_FWD(all_args)...);
        });
    }

    template < class_kind Class, typename... Args >
    bool raw_constructor_is_invocable_with(std::span<const uarg_base> args) {
        using ct = constructor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;

        return args.size() == ct::arity //
            && can_cast_all_uargs<argument_types>(args);
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

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
                using P = type_list_at_t<I, ct_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(index_constant<Is>{})...};
        }
        (std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    inline constructor_state::constructor_state(constructor_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    constructor_state_ptr constructor_state::make(metadata_map metadata) {
        constructor_state state{constructor_index{resolve_constructor_type<Class, Args...>()}, std::move(metadata)};
        state.create = make_constructor_create<Policy, Class, Args...>();
        state.create_at = make_constructor_create_at<Class, Args...>();
        state.is_invocable_with = make_constructor_is_invocable_with<Class, Args...>();
        state.arguments = make_constructor_arguments<Class, Args...>();
        return make_intrusive<constructor_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline constructor_type constructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename... Args >
    uvalue constructor::create(Args&&... args) const {
        using namespace detail;
        const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
        return state_->create(vargs);
    }

    template < typename... Args >
    std::optional<uvalue> constructor::safe_create(Args&&... args) const {
        if ( is_invocable_with(std::forward<Args>(args)...) ) {
            return create(std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    template < typename... Args >
    uvalue constructor::create_at(void* mem, Args&&... args) const {
        using namespace detail;
        const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
        return state_->create_at(mem, vargs);
    }

    template < typename... Args >
    std::optional<uvalue> constructor::safe_create_at(void* mem, Args&&... args) const {
        if ( is_invocable_with(std::forward<Args>(args)...) ) {
            return create_at(mem, std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    template < typename... Args >
    bool constructor::is_invocable_with() const noexcept {
        using namespace detail;
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
        return state_->is_invocable_with(vargs);
    }

    template < typename... Args >
    bool constructor::is_invocable_with(Args&&... args) const noexcept {
        using namespace detail;
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
        return state_->is_invocable_with(vargs);
    }

    inline argument constructor::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& constructor::get_arguments() const noexcept {
        return state_->arguments;
    }
}
