/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "data_info.hpp"
#include "evalue_info.hpp"

namespace meta_hpp
{
    class enum_info final {
    public:
        enum_info() = default;

        void merge(const enum_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        template < typename F >
        void each_evalue(F&& f) const;
    private:
        template < typename Enum > friend class enum_;

        explicit enum_info(std::string name);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct enum_info::state final {
        std::string name;
        data_info_map datas;
        evalue_info_map evalues;
    };
}

namespace meta_hpp
{
    inline void enum_info::merge(const enum_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline enum_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& enum_info::name() const noexcept {
        return state_->name;
    }
}

namespace meta_hpp
{
    template < typename F >
    void enum_info::visit(F&& f) const {
        each_data(f);
        each_evalue(f);
    }

    template < typename F >
    void enum_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    template < typename F >
    void enum_info::each_evalue(F&& f) const {
        for ( auto&& name_info : state_->evalues ) {
            std::invoke(f, name_info.second);
        }
    }
}

namespace meta_hpp
{
    inline enum_info::enum_info(std::string name)
    : state_{std::make_shared<state>(state{
        std::move(name),
        {}, {}
    })} {}
}
