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
#include "../meta_detail/value_utilities/uinst.hpp"
#include "../meta_types/member_type.hpp"

namespace meta_hpp::detail
{
    template < member_policy_family Policy, member_pointer_kind Member >
    uvalue raw_member_getter(type_registry& registry, Member member_ptr, const uinst& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        constexpr bool as_copy                                  //
            = std::is_constructible_v<uvalue, value_type>       //
           && std::is_same_v<Policy, member_policy::as_copy_t>; //

        constexpr bool as_ptr                                      //
            = std::is_same_v<Policy, member_policy::as_pointer_t>; //

        constexpr bool as_ref_wrap                                           //
            = std::is_same_v<Policy, member_policy::as_reference_wrapper_t>; //

        static_assert(as_copy || as_ptr || as_ref_wrap);

        if ( inst.is_inst_const() ) {
            META_HPP_ASSERT(                                 //
                inst.can_cast_to<const class_type>(registry) //
                && "an attempt to get a member with an incorrect instance type"
            );

            auto&& return_value = inst.cast<const class_type>(registry).*member_ptr;

            if constexpr ( as_copy ) {
                return uvalue{META_HPP_FWD(return_value)};
            }

            if constexpr ( as_ptr ) {
                return uvalue{std::addressof(return_value)};
            }

            if constexpr ( as_ref_wrap ) {
                return uvalue{std::ref(return_value)};
            }
        } else {
            META_HPP_ASSERT(                           //
                inst.can_cast_to<class_type>(registry) //
                && "an attempt to get a member with an incorrect instance type"
            );

            auto&& return_value = inst.cast<class_type>(registry).*member_ptr;

            if constexpr ( as_copy ) {
                return uvalue{META_HPP_FWD(return_value)};
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
    uerror raw_member_getter_error(type_registry& registry, const uinst_base& inst) noexcept {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;

        if ( inst.is_inst_const() ) {
            if ( !inst.can_cast_to<const class_type>(registry) ) {
                return uerror{error_code::bad_instance_cast};
            }
        } else {
            if ( !inst.can_cast_to<class_type>(registry) ) {
                return uerror{error_code::bad_instance_cast};
            }
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < member_pointer_kind Member >
    void raw_member_setter(type_registry& registry, Member member_ptr, const uinst& inst, const uarg& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( mt::is_readonly ) {
            (void)registry;
            (void)member_ptr;
            (void)inst;
            (void)arg;
            META_HPP_ASSERT(false && "an attempt to set a constant member");
        } else {
            META_HPP_ASSERT(          //
                !inst.is_inst_const() //
                && "an attempt to set a member with an const instance type"
            );

            META_HPP_ASSERT(                           //
                inst.can_cast_to<class_type>(registry) //
                && "an attempt to set a member with an incorrect instance type"
            );

            META_HPP_ASSERT(                          //
                arg.can_cast_to<value_type>(registry) //
                && "an attempt to set a member with an incorrect argument type"
            );

            inst.cast<class_type>(registry).*member_ptr = arg.cast<value_type>(registry);
        }
    }

    template < member_pointer_kind Member >
    uerror raw_member_setter_error(type_registry& registry, const uinst_base& inst, const uarg_base& arg) noexcept {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( mt::is_readonly ) {
            (void)registry;
            (void)inst;
            (void)arg;
            return uerror{error_code::bad_const_access};
        } else {
            if ( inst.is_inst_const() ) {
                return uerror{error_code::bad_const_access};
            }

            if ( !inst.can_cast_to<class_type>(registry) ) {
                return uerror{error_code::instance_type_mismatch};
            }

            if ( !arg.can_cast_to<value_type>(registry) ) {
                return uerror{error_code::argument_type_mismatch};
            }

            return uerror{error_code::no_error};
        }
    }
}

namespace meta_hpp::detail
{
    template < member_policy_family Policy, member_pointer_kind Member >
    member_state::getter_impl make_member_getter(type_registry& registry, Member member_ptr) {
        return [&registry, member_ptr](const uinst& inst) { //
            return raw_member_getter<Policy>(registry, member_ptr, inst);
        };
    }

    template < member_pointer_kind Member >
    member_state::getter_error_impl make_member_getter_error(type_registry& registry) {
        return [&registry](const uinst_base& inst) { //
            return raw_member_getter_error<Member>(registry, inst);
        };
    }

    template < member_pointer_kind Member >
    member_state::setter_impl make_member_setter(type_registry& registry, Member member_ptr) {
        return [&registry, member_ptr](const uinst& inst, const uarg& arg) { //
            return raw_member_setter(registry, member_ptr, inst, arg);
        };
    }

    template < member_pointer_kind Member >
    member_state::setter_error_impl make_member_setter_error(type_registry& registry) {
        return [&registry](const uinst_base& inst, const uarg_base& arg) { //
            return raw_member_setter_error<Member>(registry, inst, arg);
        };
    }
}

namespace meta_hpp::detail
{
    inline member_state::member_state(member_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < member_policy_family Policy, member_pointer_kind Member >
    member_state::state_ptr member_state::make(std::string name, Member member_ptr, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        member_state state{
            member_index{registry.resolve_by_type<Member>(), std::move(name)},
            std::move(metadata),
        };

        state.pointer = member_ptr;

        state.getter = make_member_getter<Policy>(registry, member_ptr);
        state.setter = make_member_setter(registry, member_ptr);
        state.getter_error = make_member_getter_error<Member>(registry);
        state.setter_error = make_member_setter_error<Member>(registry);

        return std::make_shared<member_state>(std::move(state));
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
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        return state_->getter(vinst);
    }

    template < typename Instance >
    uresult member::try_get(Instance&& instance) const {
        // doesn't actually move an 'instance', just checks conversion errors
        if ( const uerror err = check_gettable_error(META_HPP_FWD(instance)) ) {
            return err;
        }
        return get(META_HPP_FWD(instance));
    }

    template < typename Instance >
    uvalue member::operator()(Instance&& instance) const {
        return get(META_HPP_FWD(instance));
    }

    template < typename Instance, typename Value >
    void member::set(Instance&& instance, Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        const uarg vvalue{registry, META_HPP_FWD(value)};
        state_->setter(vinst, vvalue);
    }

    template < typename Instance, typename Value >
    uresult member::try_set(Instance&& instance, Value&& value) const {
        // doesn't actually move an 'instance' and a 'value', just checks conversion errors
        if ( const uerror err = check_settable_error(META_HPP_FWD(instance), META_HPP_FWD(value)) ) {
            return err;
        }
        set(META_HPP_FWD(instance), META_HPP_FWD(value));
        return uerror{error_code::no_error};
    }

    template < typename Instance, typename Value >
    void member::operator()(Instance&& instance, Value&& value) const {
        set(META_HPP_FWD(instance), META_HPP_FWD(value));
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with() const {
        return !check_gettable_error<Instance>();
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with(Instance&& instance) const {
        return !check_gettable_error(META_HPP_FWD(instance));
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with() const {
        return !check_settable_error<Instance, Value>();
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with(Instance&& instance, Value&& value) const {
        return !check_settable_error(META_HPP_FWD(instance), META_HPP_FWD(value));
    }

    template < typename Instance >
    [[nodiscard]] uerror member::check_gettable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        return state_->getter_error(vinst);
    }

    template < typename Instance >
    [[nodiscard]] uerror member::check_gettable_error(Instance&& instance) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        return state_->getter_error(vinst);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] uerror member::check_settable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        const uarg_base vvalue{registry, type_list<Value>{}};
        return state_->setter_error(vinst, vvalue);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] uerror member::check_settable_error(Instance&& instance, Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const uarg_base vvalue{registry, META_HPP_FWD(value)};
        return state_->setter_error(vinst, vvalue);
    }
}
