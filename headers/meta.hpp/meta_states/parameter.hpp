/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

namespace meta_hpp::detail
{
    template < typename Parameter >
    inline parameter_state_ptr parameter_state::make(std::string name) {
        return std::make_shared<parameter_state>(parameter_state{
            .index{parameter_index::make<Parameter>(std::move(name))},
        });
    }
}

namespace meta_hpp
{

    inline parameter::parameter(detail::parameter_state_ptr state)
    : state_{std::move(state)} {}

    inline bool parameter::is_valid() const noexcept {
        return !!state_;
    }

    inline parameter::operator bool() const noexcept {
        return is_valid();
    }

    inline const parameter_index& parameter::get_index() const noexcept {
        return state_->index;
    }

    inline const any_type& parameter::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& parameter::get_name() const noexcept {
        return state_->index.get_name();
    }
}
