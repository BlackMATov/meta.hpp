/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_types/constructor_type.hpp"

namespace meta_hpp::detail
{
    template < constructor_policy_family Policy, class_kind Class, typename... Args >
    uvalue raw_constructor_create(type_registry& registry, std::span<const uarg> args) {
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

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a constructor with incorrect argument types"
        );

        return unchecked_call_with_uargs<argument_types>(registry, args, [](auto&&... all_args) -> uvalue {
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
    uvalue raw_constructor_create_at(type_registry& registry, void* mem, std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        META_HPP_ASSERT(             //
            args.size() == ct::arity //
            && "an attempt to call a constructor with an incorrect arity"
        );

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a constructor with incorrect argument types"
        );

        return unchecked_call_with_uargs<argument_types>(registry, args, [mem](auto&&... all_args) {
            return std::construct_at(static_cast<class_type*>(mem), META_HPP_FWD(all_args)...);
        });
    }

    template < class_kind Class, typename... Args >
    uerror raw_constructor_create_error(type_registry& registry, std::span<const uarg_base> args) noexcept {
        using ct = constructor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;

        if ( args.size() != ct::arity ) {
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
    template < constructor_policy_family Policy, class_kind Class, typename... Args >
    constructor_state::create_impl make_constructor_create(type_registry& registry) {
        return [&registry](std::span<const uarg> args) { //
            return raw_constructor_create<Policy, Class, Args...>(registry, args);
        };
    }

    template < class_kind Class, typename... Args >
    constructor_state::create_at_impl make_constructor_create_at(type_registry& registry) {
        return [&registry](void* mem, std::span<const uarg> args) { //
            return raw_constructor_create_at<Class, Args...>(registry, mem, args);
        };
    }

    template < class_kind Class, typename... Args >
    constructor_state::create_error_impl make_constructor_create_error(type_registry& registry) {
        return [&registry](std::span<const uarg_base> args) { //
            return raw_constructor_create_error<Class, Args...>(registry, args);
        };
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
        }(std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    inline constructor_state::constructor_state(constructor_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < constructor_policy_family Policy, class_kind Class, typename... Args >
    constructor_state_ptr constructor_state::make(metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        constructor_state state{
            constructor_index{registry.resolve_constructor_type<Class, Args...>()},
            std::move(metadata),
        };

        state.create = make_constructor_create<Policy, Class, Args...>(registry);
        state.create_at = make_constructor_create_at<Class, Args...>(registry);
        state.create_error = make_constructor_create_error<Class, Args...>(registry);
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
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->create(vargs);
    }

    template < typename... Args >
    uresult constructor::try_create(Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
            if ( const uerror err = state_->create_error(vargs) ) {
                return err;
            }
        }

        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->create(vargs);
    }

    template < typename... Args >
    uvalue constructor::create_at(void* mem, Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->create_at(mem, vargs);
    }

    template < typename... Args >
    uresult constructor::try_create_at(void* mem, Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
            if ( const uerror err = state_->create_error(vargs) ) {
                return err;
            }
        }

        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->create_at(mem, vargs);
    }

    template < typename... Args >
    bool constructor::is_invocable_with() const noexcept {
        return !check_invocable_error<Args...>();
    }

    template < typename... Args >
    bool constructor::is_invocable_with(Args&&... args) const noexcept {
        return !check_invocable_error(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uerror constructor::check_invocable_error() const noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return state_->create_error(vargs);
    }

    template < typename... Args >
    uerror constructor::check_invocable_error(Args&&... args) const noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return state_->create_error(vargs);
    }

    inline argument constructor::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& constructor::get_arguments() const noexcept {
        return state_->arguments;
    }
}
