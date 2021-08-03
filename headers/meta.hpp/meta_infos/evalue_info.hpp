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
    class evalue_info final {
    public:
        evalue_info() = default;

        void merge(const evalue_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const class value& value() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;
    private:
        template < typename Enum > friend class evalue_;

        template < typename Enum >
        explicit evalue_info(std::string name, Enum value);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct evalue_info::state final {
        std::string name;
        class value value;
        data_info_map datas;
    };
}

namespace meta_hpp
{
    inline void evalue_info::merge(const evalue_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline evalue_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& evalue_info::name() const noexcept {
        return state_->name;
    }

    inline const class value& evalue_info::value() const noexcept {
        return state_->value;
    }
}

namespace meta_hpp
{
    template < typename F >
    void evalue_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void evalue_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }
}

namespace meta_hpp
{
    template < typename Enum >
    inline evalue_info::evalue_info(std::string name, Enum value)
    : state_{std::make_shared<state>(state{
        std::move(name),
        std::move(value),
        {}
    })} {}
}
