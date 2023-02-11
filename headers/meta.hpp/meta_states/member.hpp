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
#include "../meta_types/member_type.hpp"

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_pointer_kind Member >
    uvalue raw_member_getter(Member member_ptr, const uinst& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        constexpr bool as_copy                                  //
            = std::is_copy_constructible_v<value_type>          //
           && std::is_same_v<Policy, member_policy::as_copy_t>; //

        constexpr bool as_ptr                                      //
            = std::is_same_v<Policy, member_policy::as_pointer_t>; //

        constexpr bool as_ref_wrap                                           //
            = std::is_same_v<Policy, member_policy::as_reference_wrapper_t>; //

        static_assert(as_copy || as_ptr || as_ref_wrap);

        if ( inst.is_inst_const() ) {
            META_HPP_ASSERT(                         //
                inst.can_cast_to<const class_type>() //
                && "an attempt to get a member with an incorrect instance type"
            );

            auto&& return_value = inst.cast<const class_type>().*member_ptr;

            if constexpr ( as_copy ) {
                return uvalue{std::forward<decltype(return_value)>(return_value)};
            }

            if constexpr ( as_ptr ) {
                return uvalue{std::addressof(return_value)};
            }

            if constexpr ( as_ref_wrap ) {
                return uvalue{std::ref(return_value)};
            }
        } else {
            META_HPP_ASSERT(                   //
                inst.can_cast_to<class_type>() //
                && "an attempt to get a member with an incorrect instance type"
            );

            auto&& return_value = inst.cast<class_type>().*member_ptr;

            if constexpr ( as_copy ) {
                return uvalue{std::forward<decltype(return_value)>(return_value)};
            }

            if constexpr ( as_ptr ) {
                return uvalue{std::addressof(return_value)};
            }

            if constexpr ( as_ref_wrap ) {
                return uvalue{std::ref(return_value)};
            }
        }
    }

    template < member_pointer_kind Member >
    bool raw_member_is_gettable_with(const uinst_base& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;

        return inst.can_cast_to<const class_type>();
    }
}

namespace meta_hpp::detail
{
    template < member_pointer_kind Member >
    void raw_member_setter([[maybe_unused]] Member member_ptr, const uinst& inst, const uarg& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( std::is_const_v<value_type> ) {
            META_HPP_ASSERT(false && "an attempt to set a constant member");
        } else {
            META_HPP_ASSERT(          //
                !inst.is_inst_const() //
                && "an attempt to set a member with an const instance type"
            );

            META_HPP_ASSERT(                   //
                inst.can_cast_to<class_type>() //
                && "an attempt to set a member with an incorrect instance type"
            );

            META_HPP_ASSERT(                  //
                arg.can_cast_to<value_type>() //
                && "an attempt to set a member with an incorrect argument type"
            );

            inst.cast<class_type>().*member_ptr = arg.cast<value_type>();
        }
    }

    template < member_pointer_kind Member >
    bool raw_member_is_settable_with(const uinst_base& inst, const uarg_base& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( std::is_const_v<value_type> ) {
            return false;
        } else {
            return !inst.is_inst_const()          //
                && inst.can_cast_to<class_type>() //
                && arg.can_cast_to<value_type>(); //
        }
    }
}

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_pointer_kind Member >
    member_state::getter_impl make_member_getter(Member member_ptr) {
        return [member_ptr](const uinst& inst) { //
            return raw_member_getter<Policy>(member_ptr, inst);
        };
    }

    template < member_pointer_kind Member >
    member_state::is_gettable_with_impl make_member_is_gettable_with() {
        return &raw_member_is_gettable_with<Member>;
    }

    template < member_pointer_kind Member >
    member_state::setter_impl make_member_setter(Member member_ptr) {
        return [member_ptr](const uinst& inst, const uarg& arg) { //
            return raw_member_setter(member_ptr, inst, arg);
        };
    }

    template < member_pointer_kind Member >
    member_state::is_settable_with_impl make_member_is_settable_with() {
        return &raw_member_is_settable_with<Member>;
    }
}

namespace meta_hpp::detail
{
    inline member_state::member_state(member_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < member_policy_kind Policy, member_pointer_kind Member >
    member_state_ptr member_state::make(std::string name, Member member_ptr, metadata_map metadata) {
        member_state state{member_index{resolve_type<Member>(), std::move(name)}, std::move(metadata)};
        state.getter = make_member_getter<Policy>(member_ptr);
        state.setter = make_member_setter(member_ptr);
        state.is_gettable_with = make_member_is_gettable_with<Member>();
        state.is_settable_with = make_member_is_settable_with<Member>();
        return make_intrusive<member_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline member_type member::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& member::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename Instance >
    uvalue member::get(Instance&& instance) const {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        return state_->getter(vinst);
    }

    template < typename Instance >
    std::optional<uvalue> member::safe_get(Instance&& instance) const {
        if ( is_gettable_with(std::forward<Instance>(instance)) ) {
            return get(std::forward<Instance>(instance));
        }
        return std::nullopt;
    }

    template < typename T, typename Instance >
    T member::get_as(Instance&& instance) const {
        return get(std::forward<Instance>(instance)).template get_as<T>();
    }

    template < typename T, typename Instance >
    std::optional<T> member::safe_get_as(Instance&& instance) const {
        return safe_get(std::forward<Instance>(instance)).value_or(uvalue{}).template safe_get_as<T>();
    }

    template < typename Instance, typename Value >
    void member::set(Instance&& instance, Value&& value) const {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        const uarg vvalue{std::forward<Value>(value)};
        state_->setter(vinst, vvalue);
    }

    template < typename Instance, typename Value >
    bool member::safe_set(Instance&& instance, Value&& value) const {
        if ( is_settable_with(std::forward<Instance>(instance), std::forward<Value>(value)) ) {
            set(std::forward<Instance>(instance), std::forward<Value>(value));
            return true;
        }
        return false;
    }

    template < typename Instance >
    uvalue member::operator()(Instance&& instance) const {
        return get(std::forward<Instance>(instance));
    }

    template < typename Instance, typename Value >
    void member::operator()(Instance&& instance, Value&& value) const {
        set(std::forward<Instance>(instance), std::forward<Value>(value));
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with() const noexcept {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        return state_->is_gettable_with(vinst);
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with(Instance&& instance) const noexcept {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        return state_->is_gettable_with(vinst);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with() const noexcept {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        const uarg_base vvalue{type_list<Value>{}};
        return state_->is_settable_with(vinst, vvalue);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with(Instance&& instance, Value&& value) const noexcept {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        const uarg_base vvalue{std::forward<Value>(value)};
        return state_->is_settable_with(vinst, vvalue);
    }
}
