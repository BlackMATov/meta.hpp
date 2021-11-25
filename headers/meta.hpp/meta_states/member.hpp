/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/member_type.hpp"

namespace meta_hpp::detail
{
    template < member_kind Member >
    value raw_member_getter(Member member, const inst& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        using qualified_type = const class_type;

        if ( !inst.can_cast_to<qualified_type>() ) {
            throw std::logic_error("an attempt to get a member with an incorrect instance type");
        }

        value_type return_value{std::invoke(member, inst.cast<qualified_type>())};
        return value{std::forward<value_type>(return_value)};
    }

    template < member_kind Member >
    void raw_member_setter([[maybe_unused]] Member member, const inst& inst, const arg& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        using qualified_type = class_type;

        if constexpr ( !std::is_const_v<value_type> ) {
            if ( inst.is_const() ) {
                throw std::logic_error("an attempt to set a member with an const instance type");
            }

            if ( !inst.can_cast_to<qualified_type>() ) {
                throw std::logic_error("an attempt to set a member with an incorrect instance type");
            }

            if ( !arg.can_cast_to<value_type>() ) {
                throw std::logic_error("an attempt to set a member with an incorrect argument type");
            }

            std::invoke(member, inst.cast<qualified_type>()) = arg.cast<value_type>();
        } else {
            throw std::logic_error("an attempt to set a constant member");
        }
    }
}

namespace meta_hpp::detail
{
    template < member_kind Member >
    member_state::getter_impl make_member_getter(Member member) {
        using namespace std::placeholders;
        return std::bind(&raw_member_getter<Member>, member, _1);
    }

    template < member_kind Member >
    member_state::setter_impl make_member_setter(Member member) {
        using namespace std::placeholders;
        return std::bind(&raw_member_setter<Member>, member, _1, _2);
    }
}

namespace meta_hpp::detail
{
    template < member_kind Member >
    member_state_ptr member_state::make(std::string name, Member member) {
        member_index index{member_type_data::get<Member>(), std::move(name)};
        return std::make_shared<member_state>(std::move(index), std::move(member));
    }

    template < member_kind Member >
    member_state::member_state(member_index index, Member member)
    : index{std::move(index)}
    , getter{make_member_getter(member)}
    , setter{make_member_setter(member)} {}
}

namespace meta_hpp
{
    inline member::member(detail::member_state_ptr state)
    : state_{std::move(state)} {}

    inline bool member::is_valid() const noexcept {
        return !!state_;
    }

    inline member::operator bool() const noexcept {
        return is_valid();
    }

    inline const member_index& member::get_index() const noexcept {
        return state_->index;
    }

    inline const member_type& member::get_type() const noexcept {
        return state_->index.type;
    }

    inline const std::string& member::get_name() const noexcept {
        return state_->index.name;
    }

    template < typename Instance >
    value member::get(Instance&& instance) const {
        return state_->getter(detail::inst{std::forward<Instance>(instance)});
    }

    template < typename Instance, typename Value >
    void member::set(Instance&& instance, Value&& value) const {
        state_->setter(detail::inst{std::forward<Instance>(instance)}, detail::arg{std::forward<Value>(value)});
    }
}