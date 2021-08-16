/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_infos_fwd.hpp"

#include "base_info.hpp"
#include "ctor_info.hpp"
#include "data_info.hpp"
#include "enum_info.hpp"
#include "function_info.hpp"
#include "member_info.hpp"
#include "method_info.hpp"

namespace meta_hpp
{
    class class_info final {
    public:
        class_info() = default;

        void merge(const class_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const class_type& type() const noexcept;

        template < typename Base >
        bool is_derived_from() const;
        bool is_derived_from(any_type base) const noexcept;

        template < typename... Args >
        ctor_info get_ctor_by_args() const noexcept;
        template < std::size_t N >
        ctor_info get_ctor_by_args(const std::array<any_type, N>& args) const noexcept;
        ctor_info get_ctor_by_args(const std::vector<any_type>& args) const noexcept;
        ctor_info get_ctor_by_args(std::initializer_list<any_type> args) const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_base(F&& f) const;

        template < typename F >
        void each_class(F&& f) const;

        template < typename F >
        void each_ctor(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        template < typename F >
        void each_enum(F&& f) const;

        template < typename F >
        void each_function(F&& f) const;

        template < typename F >
        void each_member(F&& f) const;

        template < typename F >
        void each_method(F&& f) const;

        class_info get_class_by_name(std::string_view name) const noexcept;
        data_info get_data_by_name(std::string_view name) const noexcept;
        enum_info get_enum_by_name(std::string_view name) const noexcept;
        function_info get_function_by_name(std::string_view name) const noexcept;
        member_info get_member_by_name(std::string_view name) const noexcept;
        method_info get_method_by_name(std::string_view name) const noexcept;
    private:
        template < typename Class > friend class class_;

        template < typename Class >
        explicit class_info(typename_arg_t<Class>, std::string name);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct class_info::state final {
        std::string name;
        class_type type;
        base_info_map bases;
        class_info_map classes;
        ctor_info_map ctors;
        data_info_map datas;
        enum_info_map enums;
        function_info_map functions;
        member_info_map members;
        method_info_map methods;
    };
}

namespace meta_hpp
{
    inline void class_info::merge(const class_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline class_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& class_info::name() const noexcept {
        return state_->name;
    }

    inline const class_type& class_info::type() const noexcept {
        return state_->type;
    }

    template < typename Base >
    bool class_info::is_derived_from() const {
        return class_info::is_derived_from(type_db::get<Base>());
    }

    inline bool class_info::is_derived_from(any_type base) const noexcept {
        for ( auto&& id_info : state_->bases ) {
            if ( base.id() == id_info.second.type().base_class_type().id() ) {
                return true;
            }
        }
        return false;
    }

    template < typename... Args >
    ctor_info class_info::get_ctor_by_args() const noexcept {
        std::array<any_type, sizeof...(Args)> args{type_db::get<Args>()...};
        return get_ctor_by_args(args);
    }

    template < std::size_t N >
    ctor_info class_info::get_ctor_by_args(const std::array<any_type, N>& args) const noexcept {
        for ( auto&& id_info : state_->ctors ) {
            const std::vector<any_type>& ctor_args =
                id_info.second.type().argument_types();

            if ( args.size() == ctor_args.size()
                && std::equal(args.begin(), args.end(), ctor_args.begin()) )
            {
                return id_info.second;
            }
        }
        return ctor_info{};
    }

    inline ctor_info class_info::get_ctor_by_args(const std::vector<any_type>& args) const noexcept {
        for ( auto&& id_info : state_->ctors ) {
            const std::vector<any_type>& ctor_args =
                id_info.second.type().argument_types();

            if ( args.size() == ctor_args.size()
                && std::equal(args.begin(), args.end(), ctor_args.begin()) )
            {
                return id_info.second;
            }
        }
        return ctor_info{};
    }

    inline ctor_info class_info::get_ctor_by_args(std::initializer_list<any_type> args) const noexcept {
        for ( auto&& id_info : state_->ctors ) {
            const std::vector<any_type>& ctor_args =
                id_info.second.type().argument_types();

            if ( args.size() == ctor_args.size()
                && std::equal(args.begin(), args.end(), ctor_args.begin()) )
            {
                return id_info.second;
            }
        }
        return ctor_info{};
    }
}

namespace meta_hpp
{
    template < typename F >
    void class_info::visit(F&& f) const {
        each_base(f);
        each_class(f);
        each_ctor(f);
        each_data(f);
        each_enum(f);
        each_function(f);
        each_member(f);
        each_method(f);
    }

    template < typename F >
    void class_info::each_base(F&& f) const {
        for ( auto&& id_info : state_->bases ) {
            std::invoke(f, id_info.second);
        }
    }

    template < typename F >
    void class_info::each_class(F&& f) const {
        for ( auto&& name_info : state_->classes ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void class_info::each_ctor(F&& f) const {
        for ( auto&& id_info : state_->ctors ) {
            std::invoke(f, id_info.second);
        }
    }

    template < typename F >
    void class_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void class_info::each_enum(F&& f) const {
        for ( auto&& name_info : state_->enums ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void class_info::each_function(F&& f) const {
        for ( auto&& name_info : state_->functions ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void class_info::each_member(F&& f) const {
        for ( auto&& name_info : state_->members ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void class_info::each_method(F&& f) const {
        for ( auto&& name_info : state_->methods ) {
            std::invoke(f, name_info.second);
        }
    }

    inline class_info class_info::get_class_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->classes, name);
    }

    inline data_info class_info::get_data_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->datas, name);
    }

    inline enum_info class_info::get_enum_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->enums, name);
    }

    inline function_info class_info::get_function_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->functions, name);
    }

    inline member_info class_info::get_member_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->members, name);
    }

    inline method_info class_info::get_method_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->methods, name);
    }
}

namespace meta_hpp
{
    template < typename Class >
    inline class_info::class_info(typename_arg_t<Class>, std::string name)
    : state_{std::make_shared<state>(state{
        std::move(name),
        type_db::get<Class>().template as<class_type>(),
        {}, {}, {}, {}, {}, {}, {}, {}
    })} {}
}
