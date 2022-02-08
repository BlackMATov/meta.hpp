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
    template < typename Argument >
    inline argument_state_ptr argument_state::make(std::size_t position) {
        return std::make_shared<argument_state>(argument_state{
            .index{argument_index::make<Argument>(position)},
        });
    }
}

namespace meta_hpp
{

    inline argument::argument(detail::argument_state_ptr state)
    : state_{std::move(state)} {}

    inline bool argument::is_valid() const noexcept {
        return !!state_;
    }

    inline argument::operator bool() const noexcept {
        return is_valid();
    }

    inline const argument_index& argument::get_index() const noexcept {
        return state_->index;
    }

    inline const any_type& argument::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline std::size_t argument::get_position() const noexcept {
        return state_->index.get_position();
    }

    inline const std::string& argument::get_name() const noexcept {
        return state_->name;
    }
}
