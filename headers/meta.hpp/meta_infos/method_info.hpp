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
    class method_info final {
    public:
        method_info() = default;

        void merge(const method_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const method_type& type() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        data_info get_data_by_name(std::string_view name) const noexcept;
    private:
        template < typename Method > friend class method_;

        template < typename Method >
        explicit method_info(std::string name, Method instance);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct method_info::state final {
        std::string name;
        method_type type;
        data_info_map datas;
    };
}

namespace meta_hpp
{
    inline void method_info::merge(const method_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline method_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& method_info::name() const noexcept {
        return state_->name;
    }

    inline const method_type& method_info::type() const noexcept {
        return state_->type;
    }
}

namespace meta_hpp
{
    template < typename F >
    void method_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void method_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    inline data_info method_info::get_data_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->datas, name);
    }
}

namespace meta_hpp
{
    template < typename Method >
    method_info::method_info(std::string name, Method instance)
    : state_{std::make_shared<state>(state{
        std::move(name),
        type_db::get<Method>().template as<method_type>(),
        {}
    })} {
        (void)instance;
    }
}
