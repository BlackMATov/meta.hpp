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
    class evalue_info final {
    public:
        evalue_info() = default;

        void merge(const evalue_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const class value& value() const noexcept;
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
        assert(state_);
        return state_->name;
    }

    inline const class value& evalue_info::value() const noexcept {
        assert(state_);
        return state_->value;
    }

    template < typename Enum >
    inline evalue_info::evalue_info(std::string name, Enum value)
    : state_{std::make_shared<state>(state{
        std::move(name),
        std::move(value)
    })} {}
}
