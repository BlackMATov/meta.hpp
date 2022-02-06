/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/pointer_type.hpp"
#include "../meta_detail/value_utilities/arg.hpp"

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    value raw_variable_getter(const Pointer& pointer) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        constexpr bool as_copy =
            stdex::copy_constructible<data_type> &&
            stdex::same_as<Policy, variable_policy::as_copy>;

        constexpr bool as_ptr =
            stdex::same_as<Policy, variable_policy::as_pointer>;

        constexpr bool as_ref_wrap =
            stdex::same_as<Policy, variable_policy::as_reference_wrapper>;

        static_assert(as_copy || as_ptr || as_ref_wrap);

        auto&& return_value = *pointer;

        if constexpr ( as_copy ) {
            return value{std::forward<decltype(return_value)>(return_value)};
        }

        if constexpr ( as_ptr ) {
            return value{std::addressof(return_value)};
        }

        if constexpr ( as_ref_wrap) {
            return value{std::ref(return_value)};
        }
    }

    template < pointer_kind Pointer >
    void raw_variable_setter([[maybe_unused]] const Pointer& pointer, const arg& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( std::is_const_v<data_type> ) {
            throw std::logic_error("an attempt to set a constant variable");
        } else {
            if ( !arg.can_cast_to<data_type>() ) {
                throw std::logic_error("an attempt to set a variable with an incorrect argument type");
            }
            *pointer = arg.cast<data_type>();
        }
    }

    template < pointer_kind Pointer >
    bool raw_variable_is_settable_with(const arg_base& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        return !std::is_const_v<data_type>
            && arg.can_cast_to<data_type>();
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    variable_state::getter_impl make_variable_getter(Pointer pointer) {
        using namespace std::placeholders;
        return std::bind(&raw_variable_getter<Policy, Pointer>, std::move(pointer));
    }

    template < pointer_kind Pointer >
    variable_state::setter_impl make_variable_setter(Pointer pointer) {
        using namespace std::placeholders;
        return std::bind(&raw_variable_setter<Pointer>, std::move(pointer), _1);
    }

    template < pointer_kind Pointer >
    variable_state::is_settable_with_impl make_variable_is_settable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_variable_is_settable_with<Pointer>, _1);
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    variable_state_ptr variable_state::make(std::string name, Pointer pointer) {
        return std::make_shared<variable_state>(variable_state{
            .index{variable_index::make<Pointer>(std::move(name))},
            .getter{make_variable_getter<Policy>(pointer)},
            .setter{make_variable_setter(pointer)},
            .is_settable_with{make_variable_is_settable_with<Pointer>()},
        });
    }
}

namespace meta_hpp
{
    inline variable::variable(detail::variable_state_ptr state)
    : state_{std::move(state)} {}

    inline bool variable::is_valid() const noexcept {
        return !!state_;
    }

    inline variable::operator bool() const noexcept {
        return is_valid();
    }

    inline const variable_index& variable::get_index() const noexcept {
        return state_->index;
    }

    inline const pointer_type& variable::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& variable::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline value variable::get() const {
        return state_->getter();
    }

    template < typename Value >
    void variable::set(Value&& value) const {
        using namespace detail;
        const arg vvalue{std::forward<Value>(value)};
        state_->setter(vvalue);
    }

    inline value variable::operator()() const {
        return get();
    }

    template < typename Value >
    void variable::operator()(Value&& value) const {
        set(std::forward<Value>(value));
    }

    template < typename Value >
    bool variable::is_settable_with() const noexcept {
        using namespace detail;
        const arg_base vvalue{type_list<Value>{}};
        return state_->is_settable_with(vvalue);
    }

    template < typename Value >
    bool variable::is_settable_with(Value&& value) const noexcept {
        using namespace detail;
        const arg vvalue{std::forward<Value>(value)};
        return state_->is_settable_with(vvalue);
    }
}
