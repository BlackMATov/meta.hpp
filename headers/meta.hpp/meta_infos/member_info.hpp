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
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;
    private:
        template < typename Member > friend class member_;

        template < typename Member >
        explicit member_info(std::string name, Member instance);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct member_info::state final {
        std::string name;
        member_type type;
        data_info_map datas;
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
}

namespace meta_hpp
{
    template < typename Member >
    inline member_info::member_info(std::string name, Member instance)
    : state_{std::make_shared<state>(state{
        std::move(name),
        member_type{typename_arg<Member>},
        {}
    })} {
        (void)instance;
    }
}
