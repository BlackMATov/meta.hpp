/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_detail/value_utilities/uinst.hpp"
#include "../meta_types/method_type.hpp"

namespace meta_hpp::detail
{
    template < method_policy_kind Policy, method_pointer_kind Method >
    uvalue raw_method_invoke(Method method_ptr, const uinst& inst, std::span<const uarg> args) {
        using mt = method_traits<Method>;
        using return_type = typename mt::return_type;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        constexpr bool as_copy                                  //
            = std::is_copy_constructible_v<return_type>         //
           && std::is_same_v<Policy, method_policy::as_copy_t>; //

        constexpr bool as_void                                         //
            = std::is_void_v<return_type>                              //
           || std::is_same_v<Policy, method_policy::discard_return_t>; //

        constexpr bool ref_as_ptr                                                   //
            = std::is_reference_v<return_type>                                      //
           && std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>; //

        static_assert(as_copy || as_void || ref_as_ptr);

        META_HPP_THROW_IF( //
            args.size() != mt::arity,
            "an attempt to call a method with an incorrect arity"
        );

        META_HPP_THROW_IF( //
            !inst.can_cast_to<qualified_type>(),
            "an attempt to call a method with an incorrect instance type"
        );

        return std::invoke(
            [ method_ptr, &inst, args ]<std::size_t... Is>(std::index_sequence<Is...>)->uvalue {
                META_HPP_THROW_IF( //
                    !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()),
                    "an attempt to call a method with incorrect argument types"
                );

                if constexpr ( std::is_void_v<return_type> ) {
                    (inst.cast<qualified_type>().*method_ptr)(args[Is].cast<type_list_at_t<Is, argument_types>>()...);
                    return uvalue{};
                } else if constexpr ( std::is_same_v<Policy, method_policy::discard_return_t> ) {
                    std::ignore = (inst.cast<qualified_type>().*method_ptr)(args[Is].cast<type_list_at_t<Is, argument_types>>()...
                    );
                    return uvalue{};
                } else {
                    return_type&& return_value = (inst.cast<qualified_type>().*method_ptr)(
                        args[Is].cast<type_list_at_t<Is, argument_types>>()...
                    );

                    if constexpr ( ref_as_ptr ) {
                        return uvalue{std::addressof(return_value)};
                    } else {
                        return uvalue{std::forward<decltype(return_value)>(return_value)};
                    }
                }
            },
            std::make_index_sequence<mt::arity>()
        );
    }

    template < method_pointer_kind Method >
    bool raw_method_is_invocable_with(const uinst_base& inst, std::span<const uarg_base> args) {
        using mt = method_traits<Method>;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( args.size() != mt::arity ) {
            return false;
        }

        if ( !inst.can_cast_to<qualified_type>() ) {
            return false;
        }

        return std::invoke(
            [args]<std::size_t... Is>(std::index_sequence<Is...>) {
                return (... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>());
            },
            std::make_index_sequence<mt::arity>()
        );
    }
}

namespace meta_hpp::detail
{
    template < method_policy_kind Policy, method_pointer_kind Method >
    method_state::invoke_impl make_method_invoke(Method method_ptr) {
        return [method_ptr](const uinst& inst, std::span<const uarg> args) {
            return raw_method_invoke<Policy>(method_ptr, inst, args);
        };
    }

    template < method_pointer_kind Method >
    method_state::is_invocable_with_impl make_method_is_invocable_with() {
        return &raw_method_is_invocable_with<Method>;
    }

    template < method_pointer_kind Method >
    argument_list make_method_arguments() {
        using mt = method_traits<Method>;
        using mt_argument_types = typename mt::argument_types;

        return std::invoke(
            []<std::size_t... Is>(std::index_sequence<Is...>) {
                [[maybe_unused]] const auto make_argument = []<std::size_t I>(std::index_sequence<I>) {
                    using P = type_list_at_t<I, mt_argument_types>;
                    return argument{argument_state::make<P>(I, metadata_map{})};
                };
                return argument_list{make_argument(std::index_sequence<Is>{})...};
            },
            std::make_index_sequence<mt::arity>()
        );
    }
}

namespace meta_hpp::detail
{
    inline method_state::method_state(method_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < method_policy_kind Policy, method_pointer_kind Method >
    method_state_ptr method_state::make(std::string name, Method method_ptr, metadata_map metadata) {
        method_state state{method_index{resolve_type<Method>(), std::move(name)}, std::move(metadata)};
        state.invoke = make_method_invoke<Policy>(method_ptr);
        state.is_invocable_with = make_method_is_invocable_with<Method>();
        state.arguments = make_method_arguments<Method>();
        return make_intrusive<method_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline method::method(state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline bool method::is_valid() const noexcept {
        return !!state_;
    }

    inline method::operator bool() const noexcept {
        return is_valid();
    }

    inline const method_index& method::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& method::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline method_type method::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& method::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename Instance, typename... Args >
    uvalue method::invoke(Instance&& instance, Args&&... args) const {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->invoke(vinst, vargs);
        } else {
            return state_->invoke(vinst, {});
        }
    }

    template < typename Instance, typename... Args >
    uvalue method::operator()(Instance&& instance, Args&&... args) const {
        return invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with() const noexcept {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vinst, vargs);
        } else {
            return state_->is_invocable_with(vinst, {});
        }
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with(Instance&& instance, Args&&... args) const noexcept {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vinst, vargs);
        } else {
            return state_->is_invocable_with(vinst, {});
        }
    }

    inline argument method::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& method::get_arguments() const noexcept {
        return state_->arguments;
    }
}
