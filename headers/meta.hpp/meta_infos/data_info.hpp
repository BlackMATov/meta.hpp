/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_infos_fwd.hpp"

namespace meta_hpp
{
    class data_info final {
    public:
        data_info() = default;

        void merge(const data_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const class value& value() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;
    private:
        friend class data_;

        explicit data_info(std::string name, class value value);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct data_info::state final {
        std::string name;
        class value value;
        data_info_map datas;
    };
}

namespace meta_hpp
{
    inline void data_info::merge(const data_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline data_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& data_info::name() const noexcept {
        return state_->name;
    }

    inline const class value& data_info::value() const noexcept {
        return state_->value;
    }
}

namespace meta_hpp
{
    template < typename F >
    void data_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void data_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }
}

namespace meta_hpp
{
    inline data_info::data_info(std::string name, class value value)
    : state_{std::make_shared<state>(state{
        std::move(name),
        std::move(value),
        {}
    })} {}
}
