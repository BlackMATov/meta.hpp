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
    class class_info final {
    public:
        class_info() = default;

        void merge(const class_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
    private:
        template < typename Class > friend class class_;

        explicit class_info(std::string name);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct class_info::state final {
        std::string name;
    };
}

namespace meta_hpp
{
    inline void class_info::merge(const class_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline class_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& class_info::name() const noexcept {
        assert(state_);
        return state_->name;
    }

    inline class_info::class_info(std::string name)
    : state_{std::make_shared<state>(state{
        std::move(name)
    })} {}
}
