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
#include "../meta_detail/value_utilities/uinst.hpp"
#include "../meta_types/method_type.hpp"

namespace meta_hpp::detail
{
    template < method_policy_family Policy, method_pointer_kind Method >
    uvalue raw_method_invoke(type_registry& registry, Method method_ptr, const uinst& inst, std::span<const uarg> args) {
        using mt = method_traits<Method>;
        using return_type = typename mt::return_type;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        constexpr bool as_copy                          //
            = std::is_copy_constructible_v<return_type> //
           && std::is_same_v<Policy, method_policy::as_copy_t>;

        constexpr bool as_void            //
            = std::is_void_v<return_type> //
           || std::is_same_v<Policy, method_policy::discard_return_t>;

        constexpr bool ref_as_ptr              //
            = std::is_reference_v<return_type> //
           && std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>;

        static_assert(as_copy || as_void || ref_as_ptr);

        META_HPP_ASSERT(             //
            args.size() == mt::arity //
            && "an attempt to call a method with an incorrect arity"
        );

        META_HPP_ASSERT(                               //
            inst.can_cast_to<qualified_type>(registry) //
            && "an attempt to call a method with an incorrect instance type"
        );

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a method with incorrect argument types"
        );

        return call_with_uargs<argument_types>(registry, args, [method_ptr, &inst, &registry](auto&&... all_args) {
            if constexpr ( std::is_void_v<return_type> ) {
                (inst.cast<qualified_type>(registry).*method_ptr)(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( std::is_same_v<Policy, method_policy::discard_return_t> ) {
                std::ignore = (inst.cast<qualified_type>().*method_ptr)(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( !std::is_void_v<return_type> ) {
                return_type&& result = (inst.cast<qualified_type>(registry).*method_ptr)(META_HPP_FWD(all_args)...);
                return ref_as_ptr ? uvalue{std::addressof(result)} : uvalue{META_HPP_FWD(result)};
            }
        });
    }

    template < method_pointer_kind Method >
    uerror raw_method_invoke_error(type_registry& registry, const uinst_base& inst, std::span<const uarg_base> args) {
        using mt = method_traits<Method>;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( args.size() != mt::arity ) {
            return uerror{error_code::arity_mismatch};
        }

        if ( !inst.can_cast_to<qualified_type>(registry) ) {
            return uerror{error_code::instance_type_mismatch};
        }

        if ( !can_cast_all_uargs<argument_types>(registry, args) ) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < method_policy_family Policy, method_pointer_kind Method >
    method_state::invoke_impl make_method_invoke(type_registry& registry, Method method_ptr) {
        return [&registry, method_ptr](const uinst& inst, std::span<const uarg> args) {
            return raw_method_invoke<Policy>(registry, method_ptr, inst, args);
        };
    }

    template < method_pointer_kind Method >
    method_state::invoke_error_impl make_method_invoke_error(type_registry& registry) {
        return [&registry](const uinst_base& inst, std::span<const uarg_base> args) {
            return raw_method_invoke_error<Method>(registry, inst, args);
        };
    }

    template < method_pointer_kind Method >
    argument_list make_method_arguments() {
        using mt = method_traits<Method>;
        using mt_argument_types = typename mt::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
                using P = type_list_at_t<I, mt_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(index_constant<Is>{})...};
        }
        (std::make_index_sequence<mt::arity>());
    }
}

namespace meta_hpp::detail
{
    inline method_state::method_state(method_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < method_policy_family Policy, method_pointer_kind Method >
    method_state_ptr method_state::make(std::string name, Method method_ptr, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};
        method_state state{method_index{registry.resolve_type<Method>(), std::move(name)}, std::move(metadata)};
        state.invoke = make_method_invoke<Policy>(registry, method_ptr);
        state.invoke_error = make_method_invoke_error<Method>(registry);
        state.arguments = make_method_arguments<Method>();
        return make_intrusive<method_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline method_type method::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& method::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename Instance, typename... Args >
    uvalue method::invoke(Instance&& instance, Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, std::forward<Instance>(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, std::forward<Args>(args)}...};
        return state_->invoke(vinst, vargs);
    }

    template < typename Instance, typename... Args >
    uresult method::try_invoke(Instance&& instance, Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            const uinst_base vinst{registry, std::forward<Instance>(instance)};
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, std::forward<Args>(args)}...};
            if ( const uerror err = state_->invoke_error(vinst, vargs) ) {
                return err;
            }
        }

        const uinst vinst{registry, std::forward<Instance>(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, std::forward<Args>(args)}...};
        return state_->invoke(vinst, vargs);
    }

    template < typename Instance, typename... Args >
    uvalue method::operator()(Instance&& instance, Args&&... args) const {
        return invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with() const noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return !state_->invoke_error(vinst, vargs);
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with(Instance&& instance, Args&&... args) const noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, std::forward<Instance>(instance)};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, std::forward<Args>(args)}...};
        return !state_->invoke_error(vinst, vargs);
    }

    inline argument method::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& method::get_arguments() const noexcept {
        return state_->arguments;
    }
}
