/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

namespace meta_hpp
{
    class member_info final {
    public:
        member_info() = default;

        void merge(const member_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
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
        assert(state_);
        return state_->name;
    }

    template < typename Member >
    inline member_info::member_info(std::string name, Member instance)
    : state_{std::make_shared<state>(state{
        std::move(name)
    })} {
        (void)instance;
    }
}
