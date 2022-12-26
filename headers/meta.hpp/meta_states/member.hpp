/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/member_type.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_detail/value_utilities/uinst.hpp"

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_kind Member >
    uvalue raw_member_getter(const Member& member, const uinst& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        constexpr bool as_copy =
            std::copy_constructible<value_type> &&
            std::same_as<Policy, member_policy::as_copy>;

        constexpr bool as_ptr =
            std::same_as<Policy, member_policy::as_pointer>;

        constexpr bool as_ref_wrap =
            std::same_as<Policy, member_policy::as_reference_wrapper>;

        static_assert(as_copy || as_ptr || as_ref_wrap);

        if ( !inst.can_cast_to<const class_type>() ) {
            throw_exception_with("an attempt to get a member with an incorrect instance type");
        }

        if ( inst.is_inst_const() ) {
            auto&& return_value = inst.cast<const class_type>().*member;

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
            auto&& return_value = inst.cast<class_type>().*member;

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

    template < member_kind Member >
    bool raw_member_is_gettable_with(const uinst_base& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;

        return inst.can_cast_to<const class_type>();
    }
}

namespace meta_hpp::detail
{
    template < member_kind Member >
    void raw_member_setter([[maybe_unused]] const Member& member, const uinst& inst, const uarg& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( std::is_const_v<value_type> ) {
            throw_exception_with("an attempt to set a constant member");
        } else {
            if ( inst.is_inst_const() ) {
                throw_exception_with("an attempt to set a member with an const instance type");
            }

            if ( !inst.can_cast_to<class_type>() ) {
                throw_exception_with("an attempt to set a member with an incorrect instance type");
            }

            if ( !arg.can_cast_to<value_type>() ) {
                throw_exception_with("an attempt to set a member with an incorrect argument type");
            }

            inst.cast<class_type>().*member = arg.cast<value_type>();
        }
    }

    template < member_kind Member >
    bool raw_member_is_settable_with(const uinst_base& inst, const uarg_base& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        return !std::is_const_v<value_type>
            && !inst.is_inst_const()
            && inst.can_cast_to<class_type>()
            && arg.can_cast_to<value_type>();
    }
}

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_kind Member >
    member_state::getter_impl make_member_getter(Member member) {
        return [member = std::move(member)](const uinst& inst){
            return raw_member_getter<Policy>(member, inst);
        };
    }

    template < member_kind Member >
    member_state::is_gettable_with_impl make_member_is_gettable_with() {
        return &raw_member_is_gettable_with<Member>;
    }

    template < member_kind Member >
    member_state::setter_impl make_member_setter(Member member) {
        return [member = std::move(member)](const uinst& inst, const uarg& arg){
            return raw_member_setter(member, inst, arg);
        };
    }

    template < member_kind Member >
    member_state::is_settable_with_impl make_member_is_settable_with() {
        return &raw_member_is_settable_with<Member>;
    }
}

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_kind Member >
    member_state_ptr member_state::make(std::string name, Member member, metadata_map metadata) {
        return std::make_shared<member_state>(member_state{
            .index{member_index::make<Member>(std::move(name))},
            .metadata{std::move(metadata)},
            .getter{make_member_getter<Policy>(member)},
            .setter{make_member_setter(member)},
            .is_gettable_with{make_member_is_gettable_with<Member>()},
            .is_settable_with{make_member_is_settable_with<Member>()},
        });
    }
}

namespace meta_hpp
{
    inline member::member(detail::member_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline member& member::operator=(detail::member_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool member::is_valid() const noexcept {
        return !!state_;
    }

    inline member::operator bool() const noexcept {
        return is_valid();
    }

    inline const member_index& member::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& member::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const member_type& member::get_type() const noexcept {
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

    template < typename Instance, typename Value >
    void member::set(Instance&& instance, Value&& value) const {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        const uarg vvalue{std::forward<Value>(value)};
        state_->setter(vinst, vvalue);
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
