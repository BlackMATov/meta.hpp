/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_infos_fwd.hpp"

#include "data_info.hpp"

namespace meta_hpp
{
    class member_info final {
    public:
        member_info() = default;

        void merge(const member_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const member_type& type() const noexcept;

        template < typename Instance >
        value get(Instance&& instance) const;

        template < typename Instance, typename Value >
        void set(Instance&& instance, Value&& value) const;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        data_info get_data_by_name(std::string_view name) const noexcept;
    private:
        template < typename Member > friend class member_;

        template < typename Member >
        explicit member_info(std::string name, Member instance);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    using member_getter = std::function<value(const inst&)>;
    using member_setter = std::function<void(const inst&, const arg&)>;

    template < typename Member >
    value raw_member_getter(Member member, const inst& inst) {
        using mt = member_pointer_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        using qualified_type = const class_type;

        if ( !inst.can_cast_to<qualified_type>() ) {
            throw std::logic_error("an attempt to get a member with an incorrect instance type");
        }

        value_type return_value{std::invoke(member, inst.cast<qualified_type>())};
        return value{std::forward<value_type>(return_value)};
    }

    template < typename Member >
    void raw_member_setter([[maybe_unused]] Member member, const inst& inst, const arg& arg) {
        using mt = member_pointer_traits<Member>;
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

    template < typename Member >
    member_getter make_member_getter(Member member) {
        using namespace std::placeholders;
        return std::bind(&raw_member_getter<Member>, member, _1);
    }

    template < typename Member >
    member_setter make_member_setter(Member member) {
        using namespace std::placeholders;
        return std::bind(&raw_member_setter<Member>, member, _1, _2);
    }
}

namespace meta_hpp
{
    struct member_info::state final {
        std::string name;
        member_type type;
        data_info_map datas;
        detail::member_getter getter;
        detail::member_setter setter;
    };
}

namespace meta_hpp
{
    inline void member_info::merge(const member_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline member_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& member_info::name() const noexcept {
        return state_->name;
    }

    inline const member_type& member_info::type() const noexcept {
        return state_->type;
    }

    template < typename Instance >
    value member_info::get(Instance&& instance) const {
        return state_->getter(inst{std::forward<Instance>(instance)});
    }

    template < typename Instance, typename Value >
    void member_info::set(Instance&& instance, Value&& value) const {
        state_->setter(inst{std::forward<Instance>(instance)}, arg{std::forward<Value>(value)});
    }
}

namespace meta_hpp
{
    template < typename F >
    void member_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void member_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    inline data_info member_info::get_data_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->datas, name);
    }
}

namespace meta_hpp
{
    template < typename Member >
    member_info::member_info(std::string name, Member member)
    : state_{std::make_shared<state>(state{
        std::move(name),
        type_db::get<Member>().template as<member_type>(),
        {},
        detail::make_member_getter<Member>(member),
        detail::make_member_setter<Member>(member),
    })} {}
}
