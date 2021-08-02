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
    class function_info final {
    public:
        function_info() = default;

        void merge(const function_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
    private:
        template < typename Function > friend class function_;

        template < typename Function >
        explicit function_info(std::string name, Function instance);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct function_info::state final {
        std::string name;
    };
}

namespace meta_hpp
{
    inline void function_info::merge(const function_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline function_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& function_info::name() const noexcept {
        assert(state_);
        return state_->name;
    }

    template < typename Function >
    inline function_info::function_info(std::string name, Function instance)
    : state_{std::make_shared<state>(state{
        std::move(name)
    })} {
        (void)instance;
    }
}
