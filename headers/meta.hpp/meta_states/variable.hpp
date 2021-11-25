/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/pointer_type.hpp"

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    value raw_variable_getter(Pointer pointer) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        data_type return_value{*pointer};
        return value{std::forward<data_type>(return_value)};
    }

    template < pointer_kind Pointer >
    void raw_variable_setter([[maybe_unused]] Pointer pointer, const arg& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( !std::is_const_v<data_type> ) {
            if ( !arg.can_cast_to<data_type>() ) {
                throw std::logic_error("an attempt to set a variable with an incorrect argument type");
            }

            *pointer = arg.cast<data_type>();
        } else {
            throw std::logic_error("an attempt to set a constant variable");
        }
    }
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    variable_state::getter_impl make_variable_getter(Pointer pointer) {
        using namespace std::placeholders;
        return std::bind(&raw_variable_getter<Pointer>, pointer);
    }

    template < pointer_kind Pointer >
    variable_state::setter_impl make_variable_setter(Pointer pointer) {
        using namespace std::placeholders;
        return std::bind(&raw_variable_setter<Pointer>, pointer, _1);
    }
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    variable_state_ptr variable_state::make(std::string name, Pointer pointer) {
        variable_index index{pointer_type_data::get_static<Pointer>(), std::move(name)};
        return std::make_shared<variable_state>(index, pointer);
    }

    template < pointer_kind Pointer >
    variable_state::variable_state(variable_index index, Pointer pointer)
    : index{std::move(index)}
    , getter{make_variable_getter(pointer)}
    , setter{make_variable_setter(pointer)} {}
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
        return state_->index.type;
    }

    inline const std::string& variable::get_name() const noexcept {
        return state_->index.name;
    }

    inline value variable::get() const {
        return state_->getter();
    }

    template < typename Value >
    void variable::set(Value&& value) const {
        state_->setter(detail::arg{std::forward<Value>(value)});
    }
}
