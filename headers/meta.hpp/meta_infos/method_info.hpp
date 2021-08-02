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
    class method_info final {
    public:
        method_info() = default;

        void merge(const method_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
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
        assert(state_);
        return state_->name;
    }

    template < typename Method >
    inline method_info::method_info(std::string name, Method instance)
    : state_{std::make_shared<state>(state{
        std::move(name)
    })} {
        (void)instance;
    }
}
