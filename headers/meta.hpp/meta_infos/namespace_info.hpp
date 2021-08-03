/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_infos_fwd.hpp"

#include "class_info.hpp"
#include "data_info.hpp"
#include "enum_info.hpp"
#include "function_info.hpp"

namespace meta_hpp
{
    class namespace_info final {
    public:
        namespace_info() = default;

        void merge(const namespace_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_class(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        template < typename F >
        void each_enum(F&& f) const;

        template < typename F >
        void each_function(F&& f) const;

        template < typename F >
        void each_namespace(F&& f) const;

        class_info get_class_by_name(std::string_view name) const noexcept;
        data_info get_data_by_name(std::string_view name) const noexcept;
        enum_info get_enum_by_name(std::string_view name) const noexcept;
        function_info get_function_by_name(std::string_view name) const noexcept;
        namespace_info get_namespace_by_name(std::string_view name) const noexcept;
    private:
        friend class namespace_;

        explicit namespace_info(std::string name);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct namespace_info::state final {
        std::string name;
        class_info_map classes;
        data_info_map datas;
        enum_info_map enums;
        function_info_map functions;
        namespace_info_map namespaces;
    };
}

namespace meta_hpp
{
    inline void namespace_info::merge(const namespace_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline namespace_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& namespace_info::name() const noexcept {
        return state_->name;
    }
}

namespace meta_hpp
{
    template < typename F >
    void namespace_info::visit(F&& f) const {
        each_class(f);
        each_data(f);
        each_enum(f);
        each_function(f);
        each_namespace(f);
    }

    template < typename F >
    void namespace_info::each_class(F&& f) const {
        for ( auto&& name_info : state_->classes ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void namespace_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void namespace_info::each_enum(F&& f) const {
        for ( auto&& name_info : state_->enums ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void namespace_info::each_function(F&& f) const {
        for ( auto&& name_info : state_->functions ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void namespace_info::each_namespace(F&& f) const {
        for ( auto&& name_info : state_->namespaces ) {
            std::invoke(f, name_info.second);
        }
    }

    inline class_info namespace_info::get_class_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->classes, name);
    }

    inline data_info namespace_info::get_data_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->datas, name);
    }

    inline enum_info namespace_info::get_enum_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->enums, name);
    }

    inline function_info namespace_info::get_function_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->functions, name);
    }

    inline namespace_info namespace_info::get_namespace_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->namespaces, name);
    }
}

namespace meta_hpp
{
    inline namespace_info::namespace_info(std::string name)
    : state_{std::make_shared<state>(state{
        std::move(name),
        {}, {}, {}, {}, {}
    })} {}
}
