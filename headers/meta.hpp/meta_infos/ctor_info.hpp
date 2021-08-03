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
    class ctor_info final {
    public:
        ctor_info() = default;

        void merge(const ctor_info& other);
        explicit operator bool() const noexcept;

        const ctor_type& type() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;
    private:
        template < typename... Args > friend class ctor_;

        template < typename Class, typename... Args >
        explicit ctor_info(typename_arg_t<Class>, typename_arg_t<Args...>);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct ctor_info::state final {
        ctor_type type;
        data_info_map datas;
    };
}

namespace meta_hpp
{
    inline void ctor_info::merge(const ctor_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline ctor_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const ctor_type& ctor_info::type() const noexcept {
        return state_->type;
    }
}

namespace meta_hpp
{
    template < typename F >
    void ctor_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void ctor_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }
}

namespace meta_hpp
{
    template < typename Class, typename... Args >
    inline ctor_info::ctor_info(typename_arg_t<Class>, typename_arg_t<Args...>)
    : state_{std::make_shared<state>(state{
        ctor_type{typename_arg<Class>, typename_arg<Args...>},
        {}
    })} {}
}
