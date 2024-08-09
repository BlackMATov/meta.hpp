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
#include "../meta_types/pointer_type.hpp"

namespace meta_hpp::detail
{
    template < variable_policy_family Policy, pointer_kind Pointer >
    uvalue raw_variable_getter(type_registry&, Pointer variable_ptr) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        constexpr bool as_copy                                    //
            = std::is_constructible_v<uvalue, data_type>          //
           && std::is_same_v<Policy, variable_policy::as_copy_t>; //

        constexpr bool as_ptr                                        //
            = std::is_same_v<Policy, variable_policy::as_pointer_t>; //

        constexpr bool as_ref_wrap                                             //
            = std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>; //

        static_assert(as_copy || as_ptr || as_ref_wrap);

        auto&& return_value = *variable_ptr;

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

    template < pointer_kind Pointer >
    void raw_variable_setter(type_registry& registry, Pointer variable_ptr, const uarg& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( pt::is_readonly ) {
            (void)registry;
            (void)variable_ptr;
            (void)arg;
            META_HPP_ASSERT(false && "an attempt to set a constant variable");
        } else {
            META_HPP_ASSERT(                         //
                arg.can_cast_to<data_type>(registry) //
                && "an attempt to set a variable with an incorrect argument type"
            );

            *variable_ptr = arg.cast<data_type>(registry);
        }
    }

    template < pointer_kind Pointer >
    uerror raw_variable_setter_error(type_registry& registry, const uarg_base& arg) noexcept {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( pt::is_readonly ) {
            (void)registry;
            (void)arg;
            return uerror{error_code::bad_const_access};
        } else {
            if ( !arg.can_cast_to<data_type>(registry) ) {
                return uerror{error_code::argument_type_mismatch};
            }
            return uerror{error_code::no_error};
        }
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_family Policy, pointer_kind Pointer >
    variable_state::getter_impl make_variable_getter(type_registry& registry, Pointer variable_ptr) {
        return [&registry, variable_ptr]() { //
            return raw_variable_getter<Policy>(registry, variable_ptr);
        };
    }

    template < pointer_kind Pointer >
    variable_state::setter_impl make_variable_setter(type_registry& registry, Pointer variable_ptr) {
        return [&registry, variable_ptr](const uarg& arg) { //
            return raw_variable_setter(registry, variable_ptr, arg);
        };
    }

    template < pointer_kind Pointer >
    variable_state::setter_error_impl make_variable_setter_error(type_registry& registry) {
        return [&registry](const uarg_base& arg) { //
            return raw_variable_setter_error<Pointer>(registry, arg);
        };
    }
}

namespace meta_hpp::detail
{
    inline variable_state::variable_state(variable_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < variable_policy_family Policy, pointer_kind Pointer >
    variable_state::state_ptr variable_state::make(std::string name, Pointer variable_ptr, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        variable_state state{
            variable_index{registry.resolve_by_type<Pointer>(), std::move(name)},
            std::move(metadata),
        };

        state.pointer = variable_ptr;

        state.getter = make_variable_getter<Policy>(registry, variable_ptr);
        state.setter = make_variable_setter(registry, variable_ptr);
        state.setter_error = make_variable_setter_error<Pointer>(registry);

        return std::make_shared<variable_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline pointer_type variable::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& variable::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline uvalue variable::get() const {
        return state_->getter();
    }

    inline uresult variable::try_get() const {
        return state_->getter();
    }

    inline uvalue variable::operator()() const {
        return get();
    }

    template < typename Value >
    void variable::set(Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg vvalue{registry, META_HPP_FWD(value)};
        state_->setter(vvalue);
    }

    template < typename Value >
    uresult variable::try_set(Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            // doesn't actually move a 'value', just checks conversion errors
            const uarg_base vvalue{registry, META_HPP_FWD(value)};
            if ( const uerror err = state_->setter_error(vvalue) ) {
                return err;
            }
        }

        const uarg vvalue{registry, META_HPP_FWD(value)};
        state_->setter(vvalue);
        return uerror{error_code::no_error};
    }

    template < typename Value >
    void variable::operator()(Value&& value) const {
        set(META_HPP_FWD(value));
    }

    template < typename Value >
    bool variable::is_settable_with() const {
        return !check_settable_error<Value>();
    }

    template < typename Value >
    bool variable::is_settable_with(Value&& value) const {
        return !check_settable_error(META_HPP_FWD(value));
    }

    template < typename Value >
    uerror variable::check_settable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base vvalue{registry, type_list<Value>{}};
        return state_->setter_error(vvalue);
    }

    template < typename Value >
    uerror variable::check_settable_error(Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base vvalue{registry, META_HPP_FWD(value)};
        return state_->setter_error(vvalue);
    }
}
