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
    class ctor_info final {
    public:
        ctor_info() = default;

        void merge(const ctor_info& other);
        explicit operator bool() const noexcept;
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

    template < typename Class, typename... Args >
    inline ctor_info::ctor_info(typename_arg_t<Class>, typename_arg_t<Args...>)
    : state_{std::make_shared<state>(state{
    })} {}
}
